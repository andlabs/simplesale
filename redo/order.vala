// 6 september 2014

public class Order : GLib.Object {
	private Gtk.ListStore list;
	private Price subtotal;

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

	public Order()
	{
		this.list = new Gtk.ListStore(2,
			typeof (string),			// item name
			typeof (Price));			// price
		this.subtotal = 0;
	}

	public void SetupTreeView(Gtk.TreeView tv)
	{
		Gtk.CellRenderer r;
		Gtk.TreeViewColumn col;

		tv.model = this.list;
		r = new Gtk.CellRendererText();
		col = new Gtk.TreeViewColumn.with_attributes("Item", r, "text", 0);
		tv.append_column(col);
		r = new PriceRenderer();
		col = new Gtk.TreeViewColumn.with_attributes("Price", r, "price", 1);
		tv.append_column(col);
		tv.headers_visible = true;
	}

	public void Add(Gtk.TreeIter item)
	{
		string name;
		Price price;
		Gtk.TreeIter iter;

		items.get(item, 0, out name, 1, out price);
		this.list.append(out iter);
		this.list.set(iter, 0, name, 1, price);
	}
}
