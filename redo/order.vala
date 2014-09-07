// 6 september 2014

public class Order : GLib.Object {
	private Gtk.ListStore list;
	private Price subtotal;

	public string Customer {
		get;
		set;
	}

	public string TotalString {
		owned get {
			return "Total: " + this.subtotal.to_string();
		}
	}

	public string SubtotalString {
		owned get {
			return "Subtotal: " + this.subtotal.to_string();
		}
	}

	private void updateTotals()
	{
		this.notify_property("TotalString");
		this.notify_property("SubtotalString");
	}

	public Order()
	{
		this.list = new Gtk.ListStore(2,
			typeof (string),			// item name
			typeof (Price));			// price
		this.subtotal = 0;
		this.Customer = "";
	}

	public void SetupTreeView(Gtk.TreeView tv)
	{
		Gtk.CellRenderer r;
		Gtk.TreeViewColumn col;

		tv.model = this.list;
		r = new Gtk.CellRendererText();
		col = new Gtk.TreeViewColumn.with_attributes("Item", r, "text", 0);
		col.expand = true;		// make this one stretch out as wide as possible
		tv.append_column(col);
		r = new PriceRenderer();
		col = new Gtk.TreeViewColumn.with_attributes("Price", r, "price", 1);
		tv.append_column(col);
		tv.headers_visible = true;
	}

	public void Append(Gtk.TreePath path)
	{
		string name;
		Price price;
		Gtk.TreeIter iter;

		items.get_iter(out iter, path);
		items.get(iter, 0, out name, 1, out price);
		this.list.append(out iter);
		this.list.set(iter, 0, name, 1, price);
		this.subtotal += price;
		this.updateTotals();
	}

	public void Delete(Gtk.TreeIter iter)
	{
		Price price;

		this.list.get(iter, 1, out price);
		this.list.remove(iter);
		this.subtotal -= price;
		this.updateTotals();
	}
}