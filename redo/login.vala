// 6 september 2014

public class Login : Gtk.Window {
	Gtk.HeaderBar hb;
	Gtk.Button manager;

	Gtk.IconView list;
	Gtk.ScrolledWindow listScroller;

	PasswordPopover popover;

	Gtk.TreePath current;

	public Login()
	{
		GLib.Object(type: Gtk.WindowType.TOPLEVEL);
		this.title = "simplesale";
		// TODO get rid fo this
		this.destroy.connect(Gtk.main_quit);

		this.hb = new Gtk.HeaderBar();
		this.hb.title = "Log In";
		this.hb.show_close_button = false;
		this.set_titlebar(this.hb);

		this.manager = new Gtk.Button.with_label("Manager");
		this.hb.pack_end(this.manager);

		this.list = new Gtk.IconView();
		employees.SetupIconView(this.list);
		this.list.activate_on_single_click = true;
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);

		this.list.item_activated.connect((path) => {
			Gdk.Rectangle rect;

			current = path;
			if (this.list.get_cell_rect(path, null, out rect) == false)
				GLib.error("Gtk.IconView.get_cell_area() returned false on a Gtk.TreePath that it gave us");
			this.popover = new PasswordPopover(this.list);
			this.popover.pointing_to = rect;
			this.popover.Open();
		});
		this.manager.clicked.connect(() => {
			current = null;
			this.popover = new PasswordPopover(this.manager);
			this.popover.Open();
		});

		this.add(this.listScroller);
	}
}
