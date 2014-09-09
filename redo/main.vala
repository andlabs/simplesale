// 9 september 2014

private DB db;
private Items items;
private Employees employees;

public int main(string[] args)
{
	Gtk.init(ref args);
	db = new DB();
	items = new Items.FromDB(db);
	employees = new Employees.FromDB(db);
	(new Login()).show_all();
	Gtk.main();
	return 0;
}
