// 15 september 2014

[GtkTemplate(ui = "/simplesale/othersettings.ui")]
public class OtherSettings : Gtk.Window, ManagerTask {
	[GtkChild(name = "tax")]
	private Gtk.SpinButton tax;

	[GtkChild(name = "changeManagerPassword")]
	private Gtk.Button changeManagerPassword;

	public void Setup()
	{
		Gtk.HeaderBar hb;

		this.title = "simplesale";
		ScaleWindowUp(this, 1, 3);

		hb = new Gtk.HeaderBar();
		hb.show_close_button = true;
		hb.title = "Other Settings";
		this.set_titlebar(hb);
		// TODO doing the above adds lots of blank space to the bottom of the window

		this.tax.value = db.Tax();		// set initial value before connecting signal to avoid double trigger
		this.tax.changed.connect(() => {
			db.SetTax(this.tax.value);
		});

		this.changeManagerPassword.clicked.connect(() => {
			PasswordDialog prompt;
			int response;

			prompt = new PasswordDialog(this, true, "", "");
			prompt.show_all();
			for (;;) {
				response = prompt.run();
				if (response != Gtk.ResponseType.ACCEPT)
					break;
				if (db.ManagerSetPassword(prompt.Current, prompt.New) == true)
					break;
				prompt.Incorrect();
				// and continue
			}
			prompt.destroy();
		});
	}
}
