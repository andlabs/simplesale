// 12 september 2014

public class DeviceListEntry : Gtk.ListBoxRow {
	private Gtk.Grid layout;
	private Gtk.Label productLabel;
	private Gtk.Label vendorLabel;
	private Gtk.Label serialLabel;
	private Gtk.RadioButton kitchen;
	private Gtk.RadioButton receipt;

	public USBDevice Device {
		get;
		private set;
	}

	public DeviceListEntry(USBDevice device, Gtk.RadioButton kitchenGroup, Gtk.RadioButton receiptGroup, bool isKitchen, bool isReceipt)
	{
		string serialText;

		this.Device = device;

		serialText = "No serial number available.";
		if (this.Device.Serial != "")
			serialText = "Serial number: " + this.Device.Serial;

		this.layout = new Gtk.Grid();
		this.productLabel = new Gtk.Label(this.Device.Product);
		this.productLabel.attributes = new Pango.AttrList();
		this.productLabel.attributes.insert(Pango.attr_weight_new(Pango.Weight.BOLD));
		this.productLabel.wrap = true;
		this.productLabel.wrap_mode = Pango.WrapMode.WORD;
		this.productLabel.xalign = 0;
		this.layout.attach_next_to(this.productLabel, null,
			Gtk.PositionType.BOTTOM, 2, 1);
		this.vendorLabel = new Gtk.Label(this.Device.Vendor);
		this.vendorLabel.wrap = true;
		this.vendorLabel.wrap_mode = Pango.WrapMode.WORD;
		this.vendorLabel.xalign = 0;
		this.layout.attach_next_to(this.vendorLabel, this.productLabel,
			Gtk.PositionType.BOTTOM, 2, 1);
		this.serialLabel = new Gtk.Label(serialText);
		// TODO slightly gray style
		this.serialLabel.wrap = true;
		this.serialLabel.wrap_mode = Pango.WrapMode.WORD;
		this.serialLabel.xalign = 0;
		this.layout.attach_next_to(this.serialLabel, this.vendorLabel,
			Gtk.PositionType.BOTTOM, 2, 1);

		this.kitchen = new Gtk.RadioButton.with_label_from_widget(kitchenGroup, "Kitchen Printer");
		this.layout.attach_next_to(this.kitchen, this.serialLabel,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.receipt = new Gtk.RadioButton.with_label_from_widget(receiptGroup, "Receipt Printer");
		this.receipt.hexpand = true;		// force left to align left edges (TODO check to make sure this actually worked correctly)
		this.receipt.halign = Gtk.Align.START;
		this.layout.attach_next_to(this.receipt, this.kitchen,
			Gtk.PositionType.RIGHT, 1, 1);

		// set these before connecting so they don't trixxxxxxxxxxxgger another change cycle
		if (isKitchen)
			this.kitchen.active = true;
		if (isReceipt)
			this.receipt.active = true;

		// TODO connect the radio buttons here

		this.add(this.layout);
		this.show_all();
	}
}

public class DeviceList : Gtk.Grid, USBMonitor {
	private Gtk.ListBox list;
	private Gtk.ScrolledWindow listScroller;
	private Gtk.InfoBar warning;
	private Gtk.RadioButton noKitchen;
	private Gtk.RadioButton noReceipt;

	public DeviceList()
	{
		Gtk.Label warningLabel;

		GLib.Object(orientation: Gtk.Orientation.VERTICAL);

		this.list = new Gtk.ListBox();
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);
		this.listScroller.hexpand = true;
		this.listScroller.halign = Gtk.Align.FILL;
		this.listScroller.vexpand = true;
		this.listScroller.valign = Gtk.Align.FILL;

		// this is a kludge to create the radio group early
		// these won't be shown and cannot be clicked
		// they will, however, be selected by default, so we can use them to ensure devices were selected, or to update radio buttons when a device is connected
		this.noKitchen = new Gtk.RadioButton.with_label(null, "Kitchen Printer");
		this.noKitchen.sensitive = false;
		this.noKitchen.no_show_all = true;
		this.noReceipt = new Gtk.RadioButton.with_label(null, "Receipt Printer");
		this.noReceipt.sensitive = false;
		this.noReceipt.no_show_all = true;

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

		usb.RegisterMonitor(this);

		this.attach_next_to(this.warning, null,
			Gtk.PositionType.BOTTOM, 2, 1);
		this.attach_next_to(this.listScroller, this.warning,
			Gtk.PositionType.BOTTOM, 2, 1);
		this.attach_next_to(this.noKitchen, this.listScroller,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.attach_next_to(this.noReceipt, this.noKitchen,
			Gtk.PositionType.RIGHT, 1, 1);
		this.show_all();
	}

	~DeviceList()
	{
		usb.UnregisterMonitor(this);
	}

	public void DeviceConnected(USBDevice dev, bool isKitchen, bool isReceipt)
	{
		this.list.insert(new DeviceListEntry(dev, this.noKitchen, this.noReceipt, isKitchen, isReceipt), -1);
	}

	public void DeviceDisconnected(USBDevice dev)
	{
		// TODO handle kitchen/receipt properly
		// TODO explicitly set noKitchen/noReceipt above as well when this is written
		this.list.foreach((widget) => {
			if ((widget as DeviceListEntry).Device == dev)
				this.list.remove(widget);
		});
	}
}
