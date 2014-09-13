// 12 september 2014

public class DeviceListEntry : Gtk.ListBoxRow {
	private Gtk.Grid layout;
	private Gtk.Label productLabel;
	private Gtk.Label vendorLabel;
	private Gtk.Label serialLabel;
	private Gtk.RadioButton kitchen;
	private Gtk.RadioButton receipt;

	public uint16 Vendor {
		get;
		private set;
	}

	public uint16 Product {
		get;
		private set;
	}

	public string Serial {
		get;
		private set;
	}

	public DeviceListEntry(GUsb.Device device)
	{
		string vs, ps;
		string k;

		this.Vendor = device.get_vid();
		this.Product = device.get_pid();
		vs = "Vendor ID 0x%04X".printf(this.Vendor);
		ps = "Product ID 0x%04X".printf(this.Product);
		if (usbids.Vendors != null) {
			k = usbids.Vendors[this.Vendor];
			if (k == "")
				vs += " (unknown)";
			else
				vs = k;
			k = usbids.Products[this.Vendor][this.Product];
			if (k == "")
				ps += " (unknown)";
			else
				ps = k;
		}

		// note the use of k; this ensures the above fallback isn't clobbered
		// same with closing /after/ assigning ps/vs/this.Serial
		// TODO check return from device.open() and device.close()
		try {
			device.open();
			k = device.get_string_descriptor(device.get_product_index());
			ps = k;
			device.close();
		} catch (GLib.Error err) {
			// do nothing
		}
		try {
			device.open();
			k = device.get_string_descriptor(device.get_manufacturer_index());
			vs = k;
			device.close();
		} catch (GLib.Error err) {
			// do nothing
		}
		try {
			device.open();
			k = device.get_string_descriptor(device.get_serial_number_index());
			this.Serial = k;
			device.close();
		} catch (GLib.Error err) {
			// do nothing; can't do anything about it
			// we just won't check serial when comparing devices :S
			this.Serial = "";
		}

		string serialText;

		serialText = "No serial number available.";
		if (this.Serial != "")
			serialText = "Serial number: " + this.Serial;

		this.layout = new Gtk.Grid();
		this.productLabel = new Gtk.Label(ps);
		this.productLabel.attributes = new Pango.AttrList();
		this.productLabel.attributes.insert(Pango.attr_weight_new(Pango.Weight.BOLD));
		this.productLabel.wrap = true;
		this.productLabel.wrap_mode = Pango.WrapMode.WORD;
		this.productLabel.xalign = 0;
		this.layout.attach_next_to(this.productLabel, null,
			Gtk.PositionType.BOTTOM, 1, 2);
		this.vendorLabel = new Gtk.Label(vs);
		this.vendorLabel.wrap = true;
		this.vendorLabel.wrap_mode = Pango.WrapMode.WORD;
		this.vendorLabel.xalign = 0;
		this.layout.attach_next_to(this.vendorLabel, this.productLabel,
			Gtk.PositionType.BOTTOM, 1, 2);
		this.serialLabel = new Gtk.Label(serialText);
		// TODO slightly gray style
		this.serialLabel.wrap = true;
		this.serialLabel.wrap_mode = Pango.WrapMode.WORD;
		this.serialLabel.xalign = 0;
		this.layout.attach_next_to(this.serialLabel, this.vendorLabel,
			Gtk.PositionType.BOTTOM, 1, 2);

		// TODO radio buttons

		this.add(this.layout);
		this.show_all();
	}

	public bool SameAs(GUsb.Device device)
	{
		string serial = "";

		if (this.Vendor != device.get_vid())
			return false;
		if (this.Product != device.get_pid())
			return false;
		if (this.Serial == "")		// don't check; none originally available
			return true;
		try {
			device.open();		// TODO
			serial = device.get_string_descriptor(device.get_serial_number_index());
			device.close();		// TODO
		} catch (GLib.Error err) {
			// fall through; this device has no serial number, so serial will be "" and the comparison below will fail
		}
		return this.Serial != serial;
	}
}

public class DeviceList : Gtk.Grid {
	private Gtk.ListBox list;
	private Gtk.ScrolledWindow listScroller;
	private Gtk.InfoBar warning;

	private GUsb.Context context;
	private GUsb.DeviceList devlist;

	public DeviceList()
	{
		GLib.Object(orientation: Gtk.Orientation.VERTICAL);
		Gtk.Label warningLabel;

		try {
			context = new GUsb.Context();
		} catch (GLib.Error err) {
			GLib.error("error setting up USB device monitor: %s", err.message);
		}

		this.list = new Gtk.ListBox();
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);
		this.listScroller.hexpand = true;
		this.listScroller.halign = Gtk.Align.FILL;
		this.listScroller.vexpand = true;
		this.listScroller.valign = Gtk.Align.FILL;

		devlist = new GUsb.DeviceList(context);
		devlist.device_added.connect((dev) => {
			this.list.insert(new DeviceListEntry(dev), -1);
		});
		devlist.device_removed.connect((dev) => {
			this.list.foreach((widget) => {
				if ((widget as DeviceListEntry).SameAs(dev))
					this.list.remove(widget);
			});
		});
		devlist.coldplug();

		this.warning = new Gtk.InfoBar();
		this.warning.message_type = Gtk.MessageType.WARNING;
		this.warning.show_close_button = false;
		warningLabel = new Gtk.Label("Could not access a list of USB device names. Devices which do not report their own names will show raw IDs instead.");
		warningLabel.wrap = true;
		warningLabel.wrap_mode = Pango.WrapMode.WORD;
		this.warning.get_content_area().add(warningLabel);
		warningLabel.show_all();
		this.warning.hexpand = true;
		this.warning.halign = Gtk.Align.FILL;
		if (usbids.Vendors != null)
			this.warning.no_show_all = true;

		this.attach_next_to(this.warning, null,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.attach_next_to(this.listScroller, this.warning,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.show_all();
	}
}
