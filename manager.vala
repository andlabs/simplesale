// 21 june 2014
using GLib;
using Gtk;

public class Manager : GLib.Object {
	private Gtk.Window win;
	private Gtk.Grid layout;

	private struct buttonspec {
		public Type type;
		public int y;
		public int x;
		public string icon;
		public string label;
		public buttonspec(Type type, int y, int x, string icon, string label)
		{
			this.type = type;
			this.y = y;
			this.x = x;
			this.icon = icon;
			this.label = label;
		}
	}
	private static buttonspec[] buttonspecs = {
		buttonspec(typeof (Gtk.Button/*ItemEditor*/), 0, 0, "insert-object", "Item Editor"),
		buttonspec(typeof (Gtk.Button/*AccountEditor*/), 0, 1, "contact-new", "Account Editor"),
		buttonspec(typeof (Gtk.Button), 0, 2, "printer", "Device Editor"),
		buttonspec(typeof (Gtk.Button/*OtherSettings*/), 0, 3, "preferences-other", "Other Settings"),
		buttonspec(typeof (Gtk.Button), 1, 0, "list-remove", "Withdraw Money"),
		buttonspec(typeof (Gtk.Button), 1, 1, "utilities-system-monitor", "View Log"),
		buttonspec(typeof (Gtk.Button), 1, 2, "system-log-out", "Return to Login Screen"),
		buttonspec(typeof (Gtk.Button), 1, 3, "application-exit", "Quit simplesale"),
	};
	private Gtk.Button[] buttons;

	public Manager()
	{
		this.win = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
		this.win.title = "simplesale";
		// TODO remove the following
		this.win.destroy.connect(Gtk.main_quit);

//TODO		newHeaderBar("Manager Control Panel", this.win);

		this.layout = new Gtk.Grid();
		this.layout.row_homogeneous = true;
		this.layout.column_homogeneous = true;

		this.buttons = new Gtk.Button[Manager.buttonspecs.length];
		for (int i = 0; i < Manager.buttonspecs.length; i++) {
			buttonspec k = Manager.buttonspecs[i];

			this.buttons[i] = new Gtk.Button.from_icon_name(k.icon, Gtk.IconSize.DIALOG);
			this.buttons[i].label = k.label;
			this.buttons[i].image_position = Gtk.PositionType.TOP;
			this.buttons[i].always_show_image = true;
			this.buttons[i].clicked.connect(() => {
				ManagerEditor w;

				w = GLib.Object.@new(k.type) as ManagerEditor;
				this.win.hide();
				w.done.connect(() => {
					w.destroy();
					this.win.show();
				});
				w.show_all();
			});
			this.layout.attach(this.buttons[i], k.x, k.y, 1, 1);
		}

		this.win.add(this.layout);
		this.win.show_all();
	}
}

public abstract class ManagerEditor : Gtk.Widget {
	public abstract signal void done();
}

int main(string[] args)
{
	Gtk.init(ref args);
	new Manager();
	Gtk.main();
	return 0;
}
