// 17 september 2014

[GtkTemplate(ui = "/simplesale/logviewer.ui")]
public class LogViewer : Gtk.Window, ManagerTask {
	[GtkChild(name = "logbuf")]
	private Gtk.TextBuffer logbuf;

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

		Gtk.TextIter iter;

		this.logbuf.get_end_iter(out iter);
		this.logbuf.insert_with_tags_by_name(iter, "Test log entry 1\n", -1, "black");
		this.logbuf.get_end_iter(out iter);
		this.logbuf.insert_with_tags_by_name(iter, "Test log entry 2\n", -1, "red");
		this.logbuf.get_end_iter(out iter);
		this.logbuf.insert_with_tags_by_name(iter, "Test log entry 3\n", -1, "green");
		this.logbuf.get_end_iter(out iter);
		this.logbuf.insert_with_tags_by_name(iter, "Test log entry 4\n", -1, "blue");
		this.logbuf.get_end_iter(out iter);
		this.logbuf.insert_with_tags_by_name(iter, "Test log entry 5\n", -1, "purple");
	}
}
