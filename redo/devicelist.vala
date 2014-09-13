// 12 september 2014

public class DeviceListEntry : Gtk.Grid {
	private Gtk.Label productLabel;
	private Gtk.Label vendorLabel;
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

	public DeviceListEntry(uint16 vendor, uint16 product)
	{
		string vs, ps;
		string k;

		this.Vendor = vendor;
		this.Product = product;
		vs = "Vendor ID 0x%04X".printf(vendor);
		ps = "Product ID 0x%04X".printf(product);
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

		this.productLabel = new Gtk.Label(ps);
		// TODO bold
		this.attach_next_to(this.productLabel, null,
			Gtk.PositionType.BOTTOM, 1, 2);
		this.vendorLabel = new Gtk.Label(vs);
		this.attach_next_to(this.vendorLabel, this.productLabel,
			Gtk.PositionType.BOTTOM, 1, 2);

		// TODO radio buttons

		this.show_all();
	}
}

public class DeviceList : Gtk.Grid {
	private Gtk.ListBox list;
	private Gtk.ScrolledWindow listScroller;
	private Gtk.InfoBar warning;

	public DeviceList()
	{
		GLib.Object(orientation: Gtk.Orientation.VERTICAL);
		Gtk.Label warningLabel;

		this.list = new Gtk.ListBox();
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);
		this.listScroller.vexpand = true;
		this.listScroller.valign = Gtk.Align.FILL;

		this.warning = new Gtk.InfoBar();
		this.warning.message_type = Gtk.MessageType.WARNING;
		this.warning.show_close_button = false;
		warningLabel = new Gtk.Label("Could not access a list of USB device names. Raw IDs will be shown instead.");
		this.warning.get_content_area().add(warningLabel);
		warningLabel.show_all();
		if (usbids.Vendors != null)
			this.warning.no_show_all = true;

		this.attach_next_to(this.warning, null,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.attach_next_to(this.listScroller, this.warning,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.show_all();
	}
}
