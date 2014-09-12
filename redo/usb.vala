// 12 september 2014

public class USBIDs : GLib.Object {
	public GLib.HashTable<uint16, string> Vendors {
		get;
		private set;
	}

	public GLib.HashTable<uint16, GLib.HashTable<uint16, string> > Products {
		get;
		private set;
	}

	private static string[] uris = {
		// Ubuntu, Gentoo
		"file:///usr/share/misc/usb.ids",
		// openSUSE (thanks jrra), Slackware, Mageia
		"file:///usr/share/usb.ids",
		// Arch (thanks Sanky), Fedora
		"file///usr/share/hwdata/usb.ids",
		// fallback: should be last, just in case there's no internet connection
		"http://www.linux-usb.org/usb.ids",

		// TODO:
		// - OpenBSD seems to use a different approach entirely (udi_devnames)
		// - FreeBSD /usr/share/misc/usbdevs follows a different format entirely
		// 	- seems to be taken from NetBSD; I assume that has the same format (TODO)
		// - Windows? OS X? won't be relevant for simplesale but I now want to make this into a general-purpose library
	};

	private static uint uint16hash(uint16 key)
	{
		return GLib.int64_hash((int64) key);
	}

	private static bool uint16equal(uint16 a, uint16 b)
	{
		return a == b;
	}

	public USBIDs()
	{
		GLib.File file;
		GLib.FileInputStream fis;
		GLib.DataInputStream dis = null;		// keep compiler happy
		int i;
		string? line = null;					// keep compiler happy

		for (i = 0; i < USBIDs.uris.length; i++)
			try {
				file = GLib.File.new_for_uri(USBIDs.uris[i]);
				fis = file.read(null);		// not asynchronous
				dis = new GLib.DataInputStream(fis);
				line = dis.read_line();
				break;
			} catch (GLib.Error err) {		// to the next file
				continue;
			}
		if (i >= USBIDs.uris.length) {		// nothing worked
			this.Vendors = null;
			this.Products = null;
			return;
		}

		this.Vendors = new GLib.HashTable<uint16, string>(USBIDs.uint16hash, USBIDs.uint16equal);
		this.Products = new GLib.HashTable<uint16, GLib.HashTable<uint16, string> >(USBIDs.uint16hash, USBIDs.uint16equal);

		uint16 curvendor = 0;			// keep compiler happy
		uint16 curproduct;
stdout.printf("reading from %s\n", USBIDs.uris[i]);
		while (line != null) {
			if (line.length > 0)
				if ((line[0] >= '0' && line[0] <= '9') ||
					(line[0] >= 'a' && line[0] <= 'f')) {
					// digits, lowercase hex - vendor
					// TODO verify this against C99 strtol(l)
					curvendor = (uint16) uint64.parse("0x" + line[0:4]);
					this.Vendors[curvendor] = line[4:line.length].chug();
					this.Products[curvendor] = new GLib.HashTable<uint16, string>(USBIDs.uint16hash, USBIDs.uint16equal);
				} else if (line[0] == '\t') {
					if (line[1] == '\t')
						GLib.error("CONTACT ANDLABS AND TELL HIM AN INTERFACE HAS NOW BEEN DEFINED IN USB.IDS");
					// tab + digit/lowercase hex - product
					curproduct = (uint16) uint64.parse("0x" + line[1:5]);
					this.Products[curvendor][curproduct] = line[5:line.length].chug();
				} else if (line[0] >= 'A' && line[0] <= 'Z')
					// classification code; end of main list
					break;
			try {
				line = dis.read_line();
			} catch (GLib.Error err) {
				GLib.error("I/O error reading usb.ids: %s", err.message);
			}
		}
	}
}

public int main(string[] args)
{
	USBIDs ids;

	ids = new USBIDs();
	if (ids.Vendors == null) {
		stdout.printf("couldn't open vendor IDs\n");
		return 0;
	}
	ids.Vendors.foreach((k, v) => {
		stdout.printf("%x %s\n", k, v);
		ids.Products[k].foreach((k, v) => {
			stdout.printf("\t%x %s\n", k, v);
		});
	});
	return 0;
}
