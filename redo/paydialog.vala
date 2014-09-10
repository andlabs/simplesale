// 8 september 2014

public class PayDialog : Gtk.Dialog {
	private Gtk.Grid maingrid;
	private PriceEntry due;
	private PriceEntry paid;

	private Gtk.Grid numgrid;
	private Gtk.Button numbuttons[10];
	private static int[] numxpos = { 1, 0, 1, 2, 0, 1, 2, 0, 1, 2 };
	private static int[] numypos = { 3, 0, 0, 0, 1, 1, 1, 2, 2, 2 };
	private Gtk.Button dot;

	private Gtk.Button payButton;

	public PayDialog(Gtk.Window parent, Price amountDue)
	{
		// we can't chain up to gtk_dialog_new_with_buttons(); fake it
		// thanks to b4n and ebassi in irc.gimp.net/#vala
		GLib.Object(transient_for: parent, modal: true);
		this.title = "Pay";
		this.payButton = this.add_button("Pay", Gtk.ResponseType.ACCEPT) as Gtk.Button;
		this.add_button("Cancel", Gtk.ResponseType.CANCEL);

		this.maingrid = new Gtk.Grid();
		this.due = new PriceEntry();
		this.due.Price = amountDue;
		this.due.sensitive = false;
		this.maingrid.attach_next_to(this.due, null,
			Gtk.PositionType.RIGHT, 1, 1);
		this.maingrid.attach_next_to(new Gtk.Label("Amount Due"), this.due,
			Gtk.PositionType.LEFT, 1, 1);
		this.paid = new PriceEntry();
		this.maingrid.attach_next_to(this.paid, this.due,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.maingrid.attach_next_to(new Gtk.Label("Amount Paid"), this.paid,
			Gtk.PositionType.LEFT, 1, 1);

		int i;

		this.numgrid = new Gtk.Grid();
		this.numgrid.row_homogeneous = true;
		this.numgrid.column_homogeneous = true;
		for (i = 0; i < 10; i++) {
			this.numbuttons[i] = new Gtk.Button.with_label("%d".printf(i));
			this.numgrid.attach(this.numbuttons[i],
				PayDialog.numxpos[i], PayDialog.numypos[i],
				1, 1);
		}
		dot = new Gtk.Button.with_label(".");
		this.numgrid.attach_next_to(dot, this.numbuttons[0],
			Gtk.PositionType.RIGHT, 1, 1);
		this.maingrid.attach_next_to(this.numgrid, this.paid,
			Gtk.PositionType.BOTTOM, 1, 1);

		this.paid.changed.connect_after(() => {
			if (!this.paid.Valid) {
				this.payButton.sensitive = false;
				return;
			}
			if (this.paid.Price < this.due.Price) {
				this.payButton.sensitive = false;
				return;
			}
			this.payButton.sensitive = true;
		});

		this.maingrid.show_all();		// make the content visible
		this.get_content_area().add(this.maingrid);
	}

	public Price AmountPaid()
	{
		return this.paid.Price;
	}
}
