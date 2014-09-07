// 7 september 2014

public class ShiftWindow : Gtk.Window {
	Gtk.HeaderBar hb;
	Gtk.Button newOrder;
	Gtk.Button clockOut;

	Gtk.IconView list;
	Gtk.ScrolledWindow listScroller;

	private Shift s;

	public ShiftWindow() {
		GLib.Object(type: Gtk.WindowType.TOPLEVEL);
		this.title = "simplesale";
		// TODO get rid fo this
		this.destroy.connect(Gtk.main_quit);

		this.hb = new Gtk.HeaderBar();
		// TODO set this.hb.title to employee name
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

		this.s = new Shift();
		this.s.SetupIconView(this.list);

		this.add(this.listScroller);
	}
}

public int main(string[] args)
{
	Gtk.init(ref args);
	initItems();
	(new ShiftWindow()).show_all();
	Gtk.main();
	return 0;
}
