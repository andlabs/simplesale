// 9 september 2014

private Database db;
private Items items;
private Employees employees;
private USBIDs usbids;

public int main(string[] args)
{
	Gtk.init(ref args);
	db = new Database();
	items = new Items.FromDB();
	employees = new Employees.FromDB();
	usbids = new USBIDs();
	(new Login()).show_all();
	Gtk.main();
	return 0;
}
