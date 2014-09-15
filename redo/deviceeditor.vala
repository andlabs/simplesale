// 12 september 2014

public class DeviceEditor : Gtk.Window/*, ManagerTask*/ {
	Gtk.HeaderBar hb;

	Gtk.Grid layout;
	DeviceList devices;

	construct {
		Gtk.Label label;

		this.title = "simplesale";
		// TODO get rid of this
		this.destroy.connect(Gtk.main_quit);
		ScaleWindowUp(this, 2, 3);

		this.hb = new Gtk.HeaderBar();
		this.hb.show_close_button = true;
		this.hb.title = "Device Editor";
		this.set_titlebar(this.hb);

		this.layout = new Gtk.Grid();
		this.layout.border_width = 12;
		this.layout.row_spacing = 6;

		label = new Gtk.Label("Below is a list of USB devices attached to this computer. This list will refresh itself automatically. You may choose one kitchen printer and one receipt printer.");
		label.wrap = true;
		label.wrap_mode = Pango.WrapMode.WORD;
		label.hexpand = true;
		label.halign = Gtk.Align.FILL;
		label.xalign = 0;
		this.layout.attach_next_to(label, null,
			Gtk.PositionType.BOTTOM, 1, 1);

		this.devices = new DeviceList();
		this.devices.hexpand = true;
		this.devices.halign = Gtk.Align.FILL;
		this.devices.vexpand = true;
		this.devices.valign = Gtk.Align.FILL;
		this.layout.attach_next_to(this.devices, label,
			Gtk.PositionType.BOTTOM, 1, 1);

		this.add(this.layout);
	}
}
