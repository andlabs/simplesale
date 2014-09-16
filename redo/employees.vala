// 6 september 2014

public class Employees : GLib.Object, Gtk.TreeModel {
	private static Gdk.Pixbuf defaultIcon;

	static construct {
		try {
			defaultIcon = Gtk.IconTheme.get_default().load_icon("face-smile", 48, 0);
		} catch (GLib.Error err) {
			GLib.error("error loading default employee icon: %s", err.message);
		}
	}

	private static GLib.Type[] schema = {
		typeof (string),			// name
		typeof (Gdk.Pixbuf),		// icon
	};

	public void SetupIconView(Gtk.IconView iv)
	{
		iv.model = this;
		iv.text_column = 0;
		iv.pixbuf_column = 1;
		iv.item_width = 2;		// makes item widths just right (not too wide)
	}

	public void SetupTreeView(Gtk.TreeView tv)
	{
		Gtk.CellRenderer r;
		Gtk.TreeViewColumn col;

		tv.model = this;
		r = new Gtk.CellRendererText();
		col = new Gtk.TreeViewColumn.with_attributes("Name", r, "text", 0);
		tv.append_column(col);
	}

	// stamp values for Gtk.TreeIter
	private static const int goodStamp = 1000;
	private static const int badStamp = 2000;

	public void SetName(Gtk.TreeIter iter, string name)
	{
		int index;

		index = fromUserData(iter.user_data);
		if (iter.stamp != Employees.goodStamp || index < 0 || index > db.EmployeeCount())
			GLib.error("invalid or out of range iter handed to Employees.SetName()");
		db.EmployeeSetName(fromUserData(iter.user_data), name);
		this.row_changed(new Gtk.TreePath.from_indices(index), iter);
	}

	public bool CheckPassword(Gtk.TreeIter iter, string against)
	{
		int index;

		index = fromUserData(iter.user_data);
		if (iter.stamp != Employees.goodStamp || index < 0 || index > db.EmployeeCount())
			GLib.error("invalid or out of range iter handed to Employees.CheckPassword()");
		return db.EmployeeCheckPassword(fromUserData(iter.user_data), against);
	}

	public bool SetPassword(Gtk.TreeIter iter, string current, string password)
	{
		int index;

		index = fromUserData(iter.user_data);
		if (iter.stamp != Employees.goodStamp || index < 0 || index > db.EmployeeCount())
			GLib.error("invalid or out of range iter handed to Employees.SetPassword()");
		return db.EmployeeSetPassword(fromUserData(iter.user_data), current, password);
	}

	public void Append(out Gtk.TreeIter iter, string password)
	{
		int index;

		index = db.AppendEmployee("New Employee", password);
		iter.stamp = Employees.goodStamp;
		iter.user_data = toUserData(index);
		this.row_inserted(new Gtk.TreePath.from_indices(index), iter);
	}

	public void Delete(Gtk.TreeIter iter)
	{
		int index;

		index = fromUserData(iter.user_data);
		if (iter.stamp != Employees.goodStamp || index < 0 || index > db.EmployeeCount())
			GLib.error("invalid or out of range iter handed to Employees.Delete()");
		db.DeleteEmployee(fromUserData(iter.user_data));
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
		return Employees.schema.length;
	}

	public GLib.Type get_column_type(int which)
	{
		if (which < Employees.schema.length)
			return Employees.schema[which];
		return GLib.Type.INVALID;
	}

	public bool get_iter(out Gtk.TreeIter iter, Gtk.TreePath path)
	{
		int index;

		if (path.get_depth() != 1) {
			iter.stamp = Employees.badStamp;
			return false;
		}
		index = path.get_indices()[0];
		if (index < 0 || index >= db.EmployeeCount()) {
			iter.stamp = Employees.badStamp;
			return false;
		}
		iter.stamp = Employees.goodStamp;
		iter.user_data = toUserData(index);
		return true;
	}

	public Gtk.TreePath? get_path(Gtk.TreeIter iter)
	{
		if (iter.stamp != Employees.goodStamp)
			return null;
		return new Gtk.TreePath.from_indices(fromUserData(iter.user_data));
	}

	public void get_value(Gtk.TreeIter iter, int column, out GLib.Value value)
	{
		if (iter.stamp != Employees.goodStamp || column < 0 || column > schema.length)
			return;		// don't set value
		switch (column) {
		case 0:			// name
			value = db.EmployeeName(fromUserData(iter.user_data));
			break;
		case 1:
			value = Employees.defaultIcon;
			break;
		}
	}

	public bool iter_next(ref Gtk.TreeIter iter)
	{
		int index;

		if (iter.stamp != Employees.goodStamp)
			return false;
		index = fromUserData(iter.user_data);
		index++;
		if (index >= db.EmployeeCount()) {
			iter.stamp = Employees.badStamp;
			return false;
		}
		iter.user_data = toUserData(index);
		return true;
	}

	// technically not required but let's provide a fast implementation
	public bool iter_previous(ref Gtk.TreeIter iter)
	{
		int index;

		if (iter.stamp != Employees.goodStamp)
			return false;
		index = fromUserData(iter.user_data);
		index--;
		if (index < 0) {
			iter.stamp = Employees.badStamp;
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
			return db.EmployeeCount();
		return 0;
	}

	public bool iter_parent(out Gtk.TreeIter iter, Gtk.TreeIter child)
	{
		iter.stamp = Employees.badStamp;
		return false;
	}

	public bool iter_nth_child(out Gtk.TreeIter iter, Gtk.TreeIter? parent, int index)
	{
		if (parent != null) {
			iter.stamp = Employees.badStamp;
			return false;
		}
		return this.get_iter(out iter, new Gtk.TreePath.from_indices(index));
	}

	public bool iter_children(out Gtk.TreeIter iter, Gtk.TreeIter? parent)
	{
		if (parent != null || db.EmployeeCount() == 0) {
			iter.stamp = Employees.badStamp;
			return false;
		}
		iter.stamp = Employees.goodStamp;
		iter.user_data = toUserData(0);
		return true;
	}
}
