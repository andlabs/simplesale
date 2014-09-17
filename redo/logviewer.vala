// 17 september 2014

[GtkTemplate(ui = "/simplesale/logviewer.ui")]
public class LogViewer : Gtk.Window, ManagerTask {
	[GtkChild(name = "log")]
	private Gtk.TextView log;

	[GtkChild(name = "amount")]
	private PriceEntry amount;

	public void Setup()
	{
		Gtk.HeaderBar hb;

		this.title = "simplesale";
		ScaleWindowUp(this, 1, 3);

		hb = new Gtk.HeaderBar();
		hb.show_close_button = true;
		hb.title = "Log Viewer";
		this.set_titlebar(hb);
	}
}
