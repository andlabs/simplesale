// 6 september 2014

public class Items : GLib.Object, Gtk.TreeModel {
	private static GLib.Type[] schema = {
		typeof (string),				// item name
		typeof (Price),				// price
	};

	public void SetupIconView(Gtk.IconView iv)
	{
		Gtk.CellRendererText r;
		PriceRenderer r2;

		iv.model = this;
		iv.clear();
		r = new Gtk.CellRendererText();
		iv.pack_start(r, true);
		iv.set_cell_data_func(r, (layout, cell, model, iter) => {
			string name;

			items.get(iter, 0, out name);
			r.text = name;
			r.weight = Pango.Weight.BOLD;
		});
		// r2 is important here; if we reuse r the above gets applied to the below (yay not-really-closures)
		r2 = new PriceRenderer();
		iv.pack_start(r2, true);
		iv.set_attributes(r2, "Price", 1);
	}

	// TODO split this out
	public void SetupTreeView(Gtk.TreeView tv)
	{
		Gtk.CellRenderer r;
		Gtk.TreeViewColumn col;

		tv.model = this;
		r = new Gtk.CellRendererText();
		col = new Gtk.TreeViewColumn.with_attributes("Item", r, "text", 0);
		col.expand = true;		// make this one stretch out as wide as possible
		tv.append_column(col);
		r = new PriceRenderer();
		col = new Gtk.TreeViewColumn.with_attributes("Price", r, "Price", 1);
		tv.append_column(col);
		tv.headers_visible = true;
	}

	// stamp values for Gtk.TreeIter
	// TODO move them out of the methdos section in Items
	private static const int goodStamp = 1000;
	private static const int badStamp = 2000;

	public void SetName(Gtk.TreeIter iter, string name)
	{
		int index;

		index = fromUserData(iter.user_data);
		if (iter.stamp != Items.goodStamp || index < 0 || index > db.ItemCount())
			GLib.error("invalid or out of range iter handed to Items.SetName()");
		db.ItemSetName(fromUserData(iter.user_data), name);
		this.row_changed(new Gtk.TreePath.from_indices(index), iter);
	}

	public void SetPrice(Gtk.TreeIter iter, Price price)
	{
		int index;

		index = fromUserData(iter.user_data);
		if (iter.stamp != Items.goodStamp || index < 0 || index > db.ItemCount())
			GLib.error("invalid or out of range iter handed to Items.SetPrice()");
		db.ItemSetPrice(fromUserData(iter.user_data), price);
		this.row_changed(new Gtk.TreePath.from_indices(index), iter);
	}

	public void Append(out Gtk.TreeIter iter)
	{
		int index;

		index = db.AppendItem("New Item", 0);
		iter.stamp = Items.goodStamp;
		iter.user_data = toUserData(index);
		this.row_inserted(new Gtk.TreePath.from_indices(index), iter);
	}

	public void Delete(Gtk.TreeIter iter)
	{
		int index;

		index = fromUserData(iter.user_data);
		if (iter.stamp != Items.goodStamp || index < 0 || index > db.ItemCount())
			GLib.error("invalid or out of range iter handed to Items.Delete()");
		db.DeleteItem(fromUserData(iter.user_data));
		this.row_deleted(new Gtk.TreePath.from_indices(index));
	}

	// Gtk.TreeModel methods
	// these are used by Gtk.TreeView/Gtk.IconView

	public Gtk.TreeModelFlags get_flags()
	{
		return Gtk.TreeModelFlags.LIST_ONLY;
	}

	public int get_n_columns()
	{
		return Items.schema.length;
	}

	public GLib.Type get_column_type(int which)
	{
		if (which < Items.schema.length)
			return Items.schema[which];
		return GLib.Type.INVALID;
	}

	public bool get_iter(out Gtk.TreeIter iter, Gtk.TreePath path)
	{
		int index;

		if (path.get_depth() != 1) {
			iter.stamp = Items.badStamp;
			return false;
		}
		index = path.get_indices()[0];
		if (index < 0 || index >= db.ItemCount()) {
			iter.stamp = Items.badStamp;
			return false;
		}
		iter.stamp = Items.goodStamp;
		iter.user_data = toUserData(index);
		return true;
	}

	public Gtk.TreePath? get_path(Gtk.TreeIter iter)
	{
		if (iter.stamp != Items.goodStamp)
			return null;
		return new Gtk.TreePath.from_indices(fromUserData(iter.user_data));
	}

	public void get_value(Gtk.TreeIter iter, int column, out GLib.Value value)
	{
		if (iter.stamp != Items.goodStamp || column < 0 || column > schema.length)
			return;		// don't set value
		switch (column) {
		case 0:			// name
			value = db.ItemName(fromUserData(iter.user_data));
			break;
		case 1:			// price
			value = db.ItemPrice(fromUserData(iter.user_data));
			break;
		}
	}

	public bool iter_next(ref Gtk.TreeIter iter)
	{
		int index;

		if (iter.stamp != Items.goodStamp)
			return false;
		index = fromUserData(iter.user_data);
		index++;
		if (index >= db.ItemCount()) {
			iter.stamp = Items.badStamp;
			return false;
		}
		iter.user_data = toUserData(index);
		return true;
	}

	// technically not required but let's provide a fast implementation
	public bool iter_previous(ref Gtk.TreeIter iter)
	{
		int index;

		if (iter.stamp != Items.goodStamp)
			return false;
		index = fromUserData(iter.user_data);
		index--;
		if (index < 0) {
			iter.stamp = Items.badStamp;
			return false;
		}
		iter.user_data = toUserData(index);
		return true;
	}

	public bool iter_has_child(Gtk.TreeIter iter)
	{
		return false;
	}

	public int iter_n_children(Gtk.TreeIter? iter)
	{
		if (iter == null)			// number of items
			return db.ItemCount();
		return 0;
	}

	public bool iter_parent(out Gtk.TreeIter iter, Gtk.TreeIter child)
	{
		iter.stamp = Items.badStamp;
		return false;
	}

	public bool iter_nth_child(out Gtk.TreeIter iter, Gtk.TreeIter? parent, int index)
	{
		if (parent != null) {
			iter.stamp = Items.badStamp;
			return false;
		}
		return this.get_iter(out iter, new Gtk.TreePath.from_indices(index));
	}

	public bool iter_children(out Gtk.TreeIter iter, Gtk.TreeIter? parent)
	{
		if (parent != null || db.ItemCount() == 0) {
			iter.stamp = Items.badStamp;
			return false;
		}
		iter.stamp = Items.goodStamp;
		iter.user_data = toUserData(0);
		return true;
	}
}
