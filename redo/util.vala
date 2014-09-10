// 10 september 2014

public void ScaleWindowUp(Gtk.Window win, double xscale, double yscale)
{
	int width, height;
	double w, h;

	win.get_size(out width, out height);
	w = (double) width;
	h = (double) height;
	w *= xscale;
	h *=  yscale;
	win.resize((int) w, (int) h);
}

public class DualPane : GLib.Object {
	public Gtk.HeaderBar LeftHeader {
		get;
		private set;
	}

	public Gtk.HeaderBar RightHeader {
		get;
		private set;
	}

	private Gtk.Paned headerpane;
	private Gtk.Paned bodypane;

	public DualPane(Gtk.Window window)
	{
		this.LeftHeader = new Gtk.HeaderBar();
		this.RightHeader = new Gtk.HeaderBar();
		this.LeftHeader.show_close_button = false;
		this.RightHeader.show_close_button = false;
		this.headerpane = new Gtk.Paned(Gtk.Orientation.HORIZONTAL);
		this.bodypane = new Gtk.Paned(Gtk.Orientation.HORIZONTAL);
		this.headerpane.add1(this.LeftHeader);
		this.headerpane.add2(this.RightHeader);
		this.headerpane.bind_property("position",
			this.bodypane, "position",
			GLib.BindingFlags.DEFAULT | GLib.BindingFlags.BIDIRECTIONAL | GLib.BindingFlags.SYNC_CREATE);
		window.set_titlebar(this.headerpane);
		window.add(this.bodypane);
	}

	public void Add1(Gtk.Widget widget)
	{
		this.bodypane.add1(widget);
	}

	public void Add2(Gtk.Widget widget)
	{
		this.bodypane.add2(widget);
	}
}
