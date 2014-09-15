// 10 september 2014

public interface ManagerTask : Gtk.Window {
	public abstract void Setup();
}

public class ManagerToLogin : GLib.Object {
	// dummy
}

public class ManagerQuit : GLib.Object {
	// dummy
}

public class Manager : Gtk.Window {
	Gtk.HeaderBar hb;
	Gtk.Button about;

	private struct mTaskButton {
		public GLib.Type type;
		public string iconName;
		public string text;

		// kludge (TODO file bug in vala?)
		public mTaskButton(GLib.Type t, string i, string x)
		{
			this.type = t;
			this.iconName = i;
			this.text = x;
		}
	}
	private static mTaskButton[] buttonspec = {
		mTaskButton(typeof (ItemEditor), "insert-object", "Manage Items"),
		mTaskButton(typeof (EmployeeEditor), "contact-new", "Manage Employees"),
		mTaskButton(typeof (DeviceEditor), "printer", "Device Settings"),
		mTaskButton(typeof (OtherSettings), "preferences-other", "Other Settings"),
		mTaskButton(typeof (ManagerTask), "list-remove", "Withdraw Money"),
		mTaskButton(typeof (ManagerTask), "utilities-system-monitor", "View Log"),
		mTaskButton(typeof (ManagerToLogin), "system-log-out", "Return to Log In Screen"),
		mTaskButton(typeof (ManagerQuit), "application-exit", "Quit"),
	};
	private Gtk.Grid buttonGrid;
	private Gtk.Button[] buttons;

	public Manager()
	{
		GLib.Object(type: Gtk.WindowType.TOPLEVEL);
		this.title = "simplesale";
		// TODO get rid of this
		this.destroy.connect(Gtk.main_quit);

		this.hb = new Gtk.HeaderBar();
		this.hb.title = "Manager Control Panel";
		this.hb.show_close_button = false;
		this.set_titlebar(this.hb);

		this.about = new Gtk.Button.with_label("About");
		this.hb.pack_end(this.about);

		int i;
		int x, y;

		this.buttonGrid = new Gtk.Grid();
		this.buttonGrid.row_homogeneous = true;
		this.buttonGrid.column_homogeneous = true;
		this.buttons = new Gtk.Button[Manager.buttonspec.length];
		x = 0;
		y = 0;
		for (i = 0; i < Manager.buttonspec.length; i++) {
			mTaskButton tb;

			tb = Manager.buttonspec[i];
			this.buttons[i] = new Gtk.Button.from_icon_name(tb.iconName, Gtk.IconSize.DIALOG);
			this.buttons[i].always_show_image = true;
			this.buttons[i].label = tb.text;
			this.buttons[i].image_position = Gtk.PositionType.TOP;
			this.buttons[i].clicked.connect(() => {
				ManagerTask mt;

				if (tb.type == typeof (ManagerToLogin)) {
					// TODO
					return;
				}
				if (tb.type == typeof (ManagerQuit)) {
					// TODO confirm
					Gtk.main_quit();
					return;
				}
				mt = GLib.Object.@new(tb.type, type: Gtk.WindowType.TOPLEVEL) as ManagerTask;
				mt.Setup();
				// TODO
				this.hide();
				mt.show_all();
			});
			this.buttonGrid.attach(this.buttons[i],
				x, y, 1, 1);
			x++;
			if (x >= 4) {
				x = 0;
				y++;
			}
		}

		this.about.clicked.connect(() => {
			Gtk.AboutDialog about;

			about = new Gtk.AboutDialog();
			about.transient_for = this;
			about.modal = true;
			about.program_name = "simplesale";		// TODO
			about.comments = "A dead-simple point of sale system.";
			about.copyright = "Copyright ©2014 Pietro Gagliardi";
			about.authors = { "Pietro Gagliardi" };
			about.license_type = Gtk.License.MIT_X11;
			about.response.connect((response) => {
				if (response == Gtk.ResponseType.CANCEL || response == Gtk.ResponseType.DELETE_EVENT)
					about.destroy();
			});
			about.present();
		});

		this.add(this.buttonGrid);
	}
}
