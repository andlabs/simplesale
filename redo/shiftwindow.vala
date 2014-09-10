// 7 september 2014

public class ShiftWindow : Gtk.Window {
	Gtk.HeaderBar hb;
	Gtk.Button newOrder;
	Gtk.Button clockOut;

	Gtk.IconView list;
	Gtk.ScrolledWindow listScroller;

	private Shift s;

	public ShiftWindow(string name)
	{
		GLib.Object(type: Gtk.WindowType.TOPLEVEL);
		this.title = "simplesale";
		ScaleWindowUp(this, 1, 2);

		this.hb = new Gtk.HeaderBar();
		this.hb.title = name;
		this.hb.show_close_button = false;
		this.set_titlebar(this.hb);

		this.newOrder = new Gtk.Button.with_label("New Order");
		this.newOrder.get_style_context().add_class("suggested-action");
		this.hb.pack_start(this.newOrder);
		this.clockOut = new Gtk.Button.with_label("Clock Out");
		this.clockOut.get_style_context().add_class("destructive-action");
		this.hb.pack_end(this.clockOut);

		this.list = new Gtk.IconView();
		this.list.activate_on_single_click = true;
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);

		this.s = new Shift(name);
		this.s.SetupIconView(this.list);

		this.newOrder.clicked.connect(() => {
			this.continueOrder(new Order());
		});
		this.list.item_activated.connect((path) => {
			this.continueOrder(this.s.ContinueOrder(path));
		});

		this.clockOut.clicked.connect(() => {
			// TODO confirm
			this.s.ClockOut();
			this.ClockedOut();
			this.destroy();
		});

		this.add(this.listScroller);
	}

	private void continueOrder(Order order)
	{
		OrderWindow ow;

		ow = new OrderWindow(order);
		ow.PayNow.connect((o) => {
			// TODO handle drawer
			this.s.FinishOrder(o);
		});
		ow.PayLater.connect((o) => {
			this.s.AppendPending(o);
		});
		ow.show_all();
	}

	public signal void ClockedOut();
}
