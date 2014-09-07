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
	items.set(iter, 0, "Cheese Pizza", 1, 1000);
	items.append(out iter);
	items.set(iter, 0, "Cheese Slice", 1, 200);
	items.append(out iter);
	items.set(iter, 0, "Soda", 1, 125);
	items.append(out iter);
	items.set(iter, 0, "Cookie", 1, 100);
}

public void itemsSetupIconView(Gtk.IconView iv)
{
	Gtk.CellRendererText r;
	PriceRenderer r2;

	iv.model = items;
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
	iv.set_attributes(r2, "price", 1);
}
