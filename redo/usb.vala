// 16 september 2014

private GLib.Error usbOpenError()
{
	return new GLib.Error(0, 0, "GUsb.Device.open() returned false with no error");
}

public struct USBDevice {
	public uint16 VendorID;
	public uint16 ProductID;
	public string Serial;
	public GUsb.Device Device;

	public string Vendor;
	public string Product;

	public USBDevice(GUsb.Device device)
	{
		string k;

		this.VendorID = device.get_vid();
		this.ProductID = device.get_pid();
		this.Vendor = "Vendor ID 0x%04X".printf(this.VendorID);
		this.Product = "Product ID 0x%04X".printf(this.ProductID);
		if (usbids.Vendors != null) {
			k = usbids.Vendors[this.VendorID];
			if (k == "")
				this.Vendor += " (unknown)";
			else
				this.Vendor = k;
			k = usbids.Products[this.VendorID][this.ProductID];
			if (k == "")
				this.Product += " (unknown)";
			else
				this.Product = k;
		}
		// note the use of k; this ensures the above fallback isn't clobbered
		// same with closing /after/ assigning this.Product/this.Vendor/this.Serial
		// TODO check return from device.open() and device.close()
		try {
			device.open();
			k = device.get_string_descriptor(device.get_product_index());
			this.Product = k;
			device.close();
		} catch (GLib.Error err) {
			// do nothing
		}
		try {
			device.open();
			k = device.get_string_descriptor(device.get_manufacturer_index());
			this.Vendor = k;
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
			// TODO always set Serial to "", even if device.close() failed?
			this.Serial = "";
		}
		this.Device = device;
	}
}

public interface USBMonitor : GLib.Object {
	public abstract void DeviceConnected(USBDevice device, bool isKitchen, bool isReceipt);
	public abstract void DeviceDisconnected(USBDevice device);
}

public class USB : GLib.Object {
	private GUsb.Context context;
	private GUsb.DeviceList devlist;
	// wait what do you mean structs aren't boxed
	private GLib.GenericArray<USBDevice?> devices;
	private GLib.GenericArray<USBMonitor> monitors;

	private GUsb.Device kitchen;
	private GUsb.Device receipt;

	public USB()
	{
		this.devices = new GLib.GenericArray<USBDevice?>();
		this.monitors = new GLib.GenericArray<USBMonitor>();
		this.kitchen = null;
		this.receipt = null;
		try {
			this.context = new GUsb.Context();
		} catch (GLib.Error err) {
			GLib.error("error setting up USB device monitors: %s", err.message);
		}
		this.devlist = new GUsb.DeviceList(context);
		this.devlist.device_added.connect(this.added);
		this.devlist.device_removed.connect(this.removed);
		this.devlist.coldplug();
	}

	private void added(GUsb.Device dev)
	{
		USBDevice d;
		bool isKitchen = false;
		bool isReceipt = false;

		d = new USBDevice(dev);
		this.devices.add(d);
		if (this.kitchen == null && db.IsKitchenPrinter(d.VendorID, d.ProductID, d.Serial)) {
			this.kitchen = d.Device;
/* TODO this complains
			try {
				if (this.kitchen.open() == false)
					throw usbOpenError();
			} catch (GLib.Error err) {
				GLib.error("error connecting to kitchen printer: %s", err.message);
			}
*/
			isKitchen = true;
		}
		if (this.receipt == null && db.IsReceiptPrinter(d.VendorID, d.ProductID, d.Serial)) {
			this.receipt = d.Device;
/* TODO this complains
			try {
				if (this.receipt.open() == false)
					throw usbOpenError();
			} catch (GLib.Error err) {
				GLib.error("error connecting to receipt printer: %s", err.message);
			}
*/
			isReceipt = true;
		}
		this.monitors.@foreach((monitor) => {
			monitor.DeviceConnected(d, isKitchen, isReceipt);
		});
	}

	private void removed(GUsb.Device dev)
	{
		int i;

		for (i = 0; i < this.devices.length; i++)
			if (this.devices[i].Device == dev) {
				USBDevice d;

				d = this.devices[i];
				this.monitors.@foreach((monitor) => {
					monitor.DeviceDisconnected(d);
				});
				// TODO handle receipt/kitchen
				this.devices.remove_index(i);
				return;
			}
		GLib.error("unrecognized USB device %p removed", dev);
	}

	public void RegisterMonitor(USBMonitor monitor)
	{
		this.monitors.add(monitor);
		this.devices.@foreach((d) => {
			bool isKitchen, isReceipt;

			isKitchen = d.Device == this.kitchen;
			isReceipt = d.Device == this.receipt;
			monitor.DeviceConnected(d, isKitchen, isReceipt);
		});
	}

	public void UnregisterMonitor(USBMonitor monitor)
	{
		this.monitors.remove(monitor);
	}
}
