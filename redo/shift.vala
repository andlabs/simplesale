// 7 september 2014

public class Shift : GLib.Object {
	private string name;
	private Gtk.ListStore pending;
	private GLib.DateTime start;
	private GLib.Timer shift;

	public Shift(string name)
	{
		this.name = name;
		this.pending = new Gtk.ListStore(1, typeof (Order));
		this.start = new GLib.DateTime.now_utc();
		db.LogShiftStart(this.name);
		this.shift = new GLib.Timer();
		this.shift.start();

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
		r2 = new Gtk.CellRendererText();
		iv.pack_start(r2, true);
		iv.set_cell_data_func(r2, (layout, cell, model, iter) => {
			Order o;

			this.pending.get(iter, 0, out o);
			r2.text = o.TotalString;
		});
	}

	public void AppendPending(Order o)
	{
		Gtk.TreeIter iter;

		this.pending.append(out iter);
		this.pending.set(iter, 0, o);
	}

	public Order ContinueOrder(Gtk.TreePath path)
	{
		Gtk.TreeIter iter;
		Order o;

		if (this.pending.get_iter(out iter, path) == false)
			GLib.error("invalid path handed to Shift.ContinueOrder()");
		this.pending.get(iter, 0, out o);
		this.pending.remove(iter);
		return o;
	}

	public void FinishOrder(Order o)
	{
		db.LogOrder(this.name, o);
	}

	public void ClockOut()
	{
		// TODO make sure time didn't change
		db.LogShiftEnd(this.name);
	}
}
