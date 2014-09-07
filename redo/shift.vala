// 7 september 2014

public class Shift : GLib.Object {
	private Gtk.ListStore pending;

	public Shift()
	{
		this.pending = new Gtk.ListStore(1, typeof (Order));

		// add a sample order to test
		var o = new Order();
		o.Append(new Gtk.TreePath.from_indices(1));
		o.Customer = "Test Customer";
		Gtk.TreeIter iter;
		this.pending.append(out iter);
		this.pending.set(iter, 0, o);
	}

	public void SetupIconView(Gtk.IconView iv)
	{
		Gtk.CellRendererText r, r2;

		iv.model = this.pending;
		iv.clear();
		r = new Gtk.CellRendererText();
		iv.pack_start(r, true);
		iv.set_cell_data_func(r, (layout, cell, model, iter) => {
			Order o;

			this.pending.get(iter, 0, out o);
			r.text = o.Customer;
			r.weight = Pango.Weight.BOLD;
		});
		// r2 is important here; if we reuse r the above gets applied to the below (yay not-really-closures)
		r2 = new PriceRenderer();
		iv.pack_start(r2, true);
		iv.set_cell_data_func(r2, (layout, cell, model, iter) => {
			Order o;

			this.pending.get(iter, 0, out o);
			r2.text = o.TotalString;
		});
	}
}
