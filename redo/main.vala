// 9 september 2014

private Database db;
private Items items;
private Employees employees;
private USBIDs usbids;
private USB usb;

public int main(string[] args)
{
	Gtk.init(ref args);
	db = new Database();
	items = new Items();
	employees = new Employees();
	usbids = new USBIDs();
	usb = new USB();
	(new Login()).show_all();
	Gtk.main();
	return 0;
}
