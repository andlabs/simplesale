// 6 september 2014

private Gtk.ListStore accounts;
private Gdk.Pixbuf accountIcon;

public void initAccounts()
{
	accounts = new Gtk.ListStore(3,
		typeof (string),			// name
		typeof (string),			// password
		typeof (Gdk.Pixbuf));		// icon
	try {
		accountIcon = Gtk.IconTheme.get_default().load_icon("face-smile", 48, 0);
	} catch (GLib.Error err) {
		GLib.error("error loading default account icon: %s", err.message);
	}

	// now let's fill in some defaults
	Gtk.TreeIter iter;

	accounts.append(out iter);
	accounts.set(iter, 0, "Name 1", 2, accountIcon);
	accounts.append(out iter);
	accounts.set(iter, 0, "Name 2", 2, accountIcon);
	accounts.append(out iter);
	accounts.set(iter, 0, "Name 3", 2, accountIcon);
}

public void accountsSetupIconView(Gtk.IconView iv)
{
	iv.model = accounts;
	iv.text_column = 0;
	iv.pixbuf_column = 2;
	iv.item_width = 2;		// makes item widths just right (not too wide)
}
