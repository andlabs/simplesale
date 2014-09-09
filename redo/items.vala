// 6 september 2014

public class Items : Gtk.ListStore {
	private static GLib.Type[] schema = {
		typeof (string),				// item name
		typeof (Price),				// price
	};

	public Items()
	{
		this.set_column_types(Items.schema);
	}

	public Items.FromDB()
	{
		string name;
		Price p;
		Gtk.TreeIter iter;

		this();
		db.BeginGetItems();
		while (db.GetNextItem(out name, out price)) {
			this.append(out iter);
			this.set(iter, 0, name, 1, price);
		}
		db.EndGetItems();
	}

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
}
