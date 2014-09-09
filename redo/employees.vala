// 6 september 2014

public class Employees : Gtk.ListStore {
	private static Gdk.Pixbuf defaultIcon;

	static construct {
		try {
			defaultIcon = Gtk.IconTheme.get_default().load_icon("face-smile", 48, 0);
		} catch (GLib.Error err) {
			GLib.error("error loading default employee icon: %s", err.message);
		}
	}

	private static GLib.Type[] schema = {
		typeof (string),			// name
		typeof (string),			// password
		typeof (Gdk.Pixbuf),		// icon
	};

	public Employees()
	{
		this.set_column_types(Employees.schema);
	}

	public Employees.FromDB()
	{
		string name;
		string password;
		Gdk.Pixbuf icon;
		Gtk.TreeIter iter;

		this();
		db.BeginGetEmployees();
		while (db.GetNextEmployee(out name, out password, out icon)) {
			this.append(out iter);
			if (icon == null)
				icon = Employees.defaultIcon;
			this.set(iter, 0, name, 1, password, 2, icon);
		}
		db.EndGetEmployees();
	}

	public void SetupIconView(Gtk.IconView iv)
	{
		iv.model = this;
		iv.text_column = 0;
		iv.pixbuf_column = 2;
		iv.item_width = 2;		// makes item widths just right (not too wide)
	}
}
