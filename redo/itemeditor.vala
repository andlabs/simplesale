// 10 september 2014

[GtkTemplate(ui = "/simplesale/itemeditor.ui")]
public class ItemEditor : Gtk.Window, ManagerTask {
	[GtkChild(name = "list")]
	private Gtk.TreeView list;

	[GtkChild(name = "listSelection")]
	private Gtk.TreeSelection listSelection;

	[GtkChild(name = "addButton")]
	private Gtk.ToolButton addButton;

	[GtkChild(name = "removeButton")]
	private Gtk.ToolButton removeButton;

	[GtkChild(name = "search")]
	private Gtk.SearchEntry search;

	[GtkChild(name = "name")]
	private new Gtk.Entry name;

	[GtkChild(name = "price")]
	private PriceEntry price;

	private bool selected;
	private Gtk.TreeIter selection;

	private ulong nameChangedHandler;
	private ulong priceChangedHandler;

	public void Setup()
	{
		Gtk.HeaderBar hb;

		this.title = "simplesale";
		// TODO get rid of this
		this.destroy.connect(Gtk.main_quit);
		ScaleWindowUp(this, 1, 3);

		hb = new Gtk.HeaderBar();
		hb.show_close_button = true;
		hb.title = "Item Editor";
		this.set_titlebar(hb);

		items.SetupTreeView(this.list);
		this.list.set_search_entry(this.search);

		// these two must come before the selected ones so the variables can be set for the initial signal
		this.nameChangedHandler = this.name.changed.connect(() => {
			if (!this.selected)
				GLib.error("item name changed with no item selected");
			items.set(this.selection, 0, this.name.text);
		});
		this.priceChangedHandler = this.price.changed.connect(() => {
			if (!this.selected)
				GLib.error("item price changed with no item selected");
			if (this.price.Valid)
				items.set(this.selection, 1, this.price.Price);
		});

		this.selected = false;
		this.list.get_selection().changed.connect(() => {
			this.selected = this.list.get_selection().get_selected(null, out this.selection);
			this.name.sensitive = this.selected;
			this.price.sensitive = this.selected;
			this.removeButton.sensitive = this.selected;
			if (this.selected) {
				string n;
				Price p;

				items.get(this.selection, 0, out n, 1, out p);
				this.name.text = n;
				this.price.Price = p;
			} else {
				// prevent the GLib.error()s above from triggering when deselecting
				GLib.SignalHandler.block(this.name, this.nameChangedHandler);
				GLib.SignalHandler.block(this.price, this.priceChangedHandler);
				this.name.text = "";
				this.price.Price = 0;
				GLib.SignalHandler.unblock(this.name, this.nameChangedHandler);
				GLib.SignalHandler.unblock(this.price, this.priceChangedHandler);
			}
		});
		// and set initial value
		this.list.get_selection().changed();

		this.addButton.clicked.connect(() => {
			Gtk.TreeIter iter;

			items.append(out iter);
			items.set(iter, 0, "New Item");
			this.list.get_selection().select_iter(iter);
		});
		this.removeButton.clicked.connect(() => {
			if (!this.selected)
				GLib.error("remove item button clicked without any item selected");
			items.remove(this.selection);
		});
	}
}
