// 17 september 2014

[GtkTemplate(ui = "/simplesale/addremovemoney.ui")]
public class AddRemoveMoney : Gtk.Window, ManagerTask {
	[GtkChild(name = "current")]
	private PriceEntry current;

	[GtkChild(name = "changeAmount")]
	private PriceEntry changeAmount;

	[GtkChild(name = "addAmount")]
	private Gtk.RadioButton addAmount;

	[GtkChild(name = "removeAmount")]
	private Gtk.RadioButton removeAmount;

	[GtkChild(name = "change")]
	private Gtk.Button change;

	public void Setup()
	{
		Gtk.HeaderBar hb;

		this.title = "simplesale";
		ScaleWindowUp(this, 1, 3);

		hb = new Gtk.HeaderBar();
		hb.show_close_button = true;
		hb.title = "Add/Remove Money";
		this.set_titlebar(hb);

		// Glade doesn't let me set these two
		addAmount.image = new Gtk.Image.from_icon_name("list-add-symbolic", Gtk.IconSize.MENU);
		removeAmount.image = new Gtk.Image.from_icon_name("list-remove-symbolic", Gtk.IconSize.MENU);
	}
}
