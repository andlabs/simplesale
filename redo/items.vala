// 6 september 2014

private Gtk.ListStore items;

public void initItems()
{
	items = new Gtk.ListStore(2,
		typeof (string),				// item name
		typeof (Price));				// price

	// some sample items
	Gtk.TreeIter iter;

	items.append(out iter);
	items.set(iter, 0, "Cheese Pizza", 1000);
	items.append(out iter);
	items.set(iter, 0, "Cheese Slice", 0200);
	items.append(out iter);
	items.set(iter, 0, "Soda", 125);
	items.append(out iter);
	items.set(iter, 0, "Cookie", 100);
}

public void itemsSetupIconView(Gtk.IconView iv)
{
	Gtk.CellRenderer r;

	iv.model = items;
	iv.clear();
	r = new Gtk.CellRendererText();
	iv.pack_start(r, true);
	iv.set_attributes(r, "text", 0);
	// TODO price renderer
}
