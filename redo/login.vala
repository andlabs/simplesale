// 6 september 2014

public class Login : Gtk.Window {
	Gtk.HeaderBar hb;
	Gtk.Button manager;

	Gtk.IconView list;
	Gtk.ScrolledWindow listScroller;

	Gtk.Popover popover;
	Gtk.Grid playout;
	Gtk.Entry password;
	Gtk.Button login;
	Gtk.Label incorrect;

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
		accountsSetupIconView(this.list);
		this.list.activate_on_single_click = true;
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);

		this.popover = new Gtk.Popover(this.list);
		this.playout = new Gtk.Grid();
		this.password = new Gtk.Entry();
		this.password.visibility = false;
		this.password.caps_lock_warning = true;
		this.password.placeholder_text = "Enter password";
		this.playout.attach_next_to(this.password, null,
			Gtk.PositionType.RIGHT, 1, 1);
		this.login = new Gtk.Button.with_label("Log In");
		this.login.get_style_context().add_class("suggested-action");
		this.playout.attach_next_to(this.login, this.password,
			Gtk.PositionType.RIGHT, 1, 1);
		this.incorrect = new Gtk.Label("Password incorrect.");
		this.incorrect.no_show_all = true;
		this.playout.attach_next_to(this.incorrect, this.password,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.popover.add(this.playout);

		this.list.item_activated.connect((path) => {
			Gdk.Rectangle rect;

			current = path;
			if (this.list.get_cell_rect(path, null, out rect) == false)
				GLib.error("Gtk.IconView.get_cell_area() returned false on a Gtk.TreePath that it gave us");
			this.popover.relative_to = this.list;
			this.popover.pointing_to = rect;
			this.incorrect.hide();
			this.popover.show_all();
		});
		this.manager.clicked.connect(() => {
			current = null;
			this.popover.relative_to = this.manager;
			this.incorrect.hide();
			this.popover.show_all();
		});

		this.add(this.listScroller);
	}
}

public int main(string[] args)
{
	Gtk.init(ref args);
	initAccounts();
	(new Login()).show_all();
	Gtk.main();
	return 0;
}
