// 6 september 2014

public Gtk.ListStore newOrder()
{
	return new Gtk.ListStore(2,
		typeof (string),			// item name
		typeof (Price));			// price
}

public void orderSetupTreeView(Gtk.ListStore o, Gtk.TreeView tv)
{
	Gtk.CellRenderer r;
	Gtk.TreeViewColumn col;

	tv.model = o;
	r = new Gtk.CellRendererText();
	col = new Gtk.TreeViewColumn.with_attributes("Item", r, "text", 0);
	tv.append_column(col);
	r = new PriceRenderer();
	col = new Gtk.TreeViewColumn.with_attributes("Price", r, "price", 1);
	tv.append_column(col);
	tv.headers_visible = true;
}
