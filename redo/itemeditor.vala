// 10 september 2014

public class ItemEditor : ManagerTask {
	private DualPane dp;
	private Gtk.SearchEntry search;
	private Gtk.Button add;
	private Gtk.Button remove;
	private Gtk.Button save;
	private Gtk.Button cancel;

	private Items items;
	private Gtk.TreeView list;
	private Gtk.ScrolledWindow listScroller;

	private Gtk.Grid editorGrid;
	private Gtk.Entry name;
	private PriceEntry price;

	private bool selected;
	private Gtk.TreeIter selection;

	private ulong nameChangedHandler;
	private ulong priceChangedHandler;

	// we can't use a normal constructor here because Vala constructor != GObject constructor
	// thanks to Lethalman, flo, and mezen in irc.gimp.net/#vala
	construct {
		Gtk.Label label;

		this.title = "simplesale";
		// TODO get rid of this
		this.destroy.connect(Gtk.main_quit);
		ScaleWindowUp(this, 5, 3);

		this.dp = new DualPane(this);
		this.dp.RightHeader.title = "Item Editor";
		this.search = new Gtk.SearchEntry();
		this.search.placeholder_text = "Find existing item";
		this.dp.LeftHeader.pack_start(this.search);
		// pack_end() is reversed
		this.remove = new Gtk.Button.from_icon_name("list-remove-symbolic", Gtk.IconSize.BUTTON);
		this.dp.LeftHeader.pack_end(this.remove);
		this.add = new Gtk.Button.from_icon_name("list-add-symbolic", Gtk.IconSize.BUTTON);
		this.dp.LeftHeader.pack_end(this.add);
		this.save = new Gtk.Button.with_label("Save Changes");
		this.save.get_style_context().add_class("suggested-action");
		this.dp.RightHeader.pack_start(this.save);
		this.cancel = new Gtk.Button.with_label("Cancel Changes");
		this.cancel.get_style_context().add_class("destructive-action");
		this.dp.RightHeader.pack_end(this.cancel);

		this.items = new Items.FromDB();
		this.list = new Gtk.TreeView();
		this.items.SetupTreeView(this.list);
		this.list.set_search_entry(this.search);
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);
		this.dp.Add1(this.listScroller);

		this.editorGrid = new Gtk.Grid();
		this.editorGrid.border_width = 12;
		this.editorGrid.row_spacing = 6;
		this.editorGrid.column_spacing = 12;
		this.name = new Gtk.Entry();
		this.name.hexpand = true;
		this.name.halign = Gtk.Align.FILL;
		this.editorGrid.attach_next_to(this.name, null,
			Gtk.PositionType.TOP, 1, 1);
		label = new Gtk.Label("Name");
		label.xalign = 0;
		this.editorGrid.attach_next_to(label, this.name,
			Gtk.PositionType.LEFT, 1, 1);
		this.price = new PriceEntry();
		this.price.hexpand = true;
		this.price.halign = Gtk.Align.FILL;
		this.editorGrid.attach_next_to(this.price, this.name,
			Gtk.PositionType.BOTTOM, 1, 1);
		label = new Gtk.Label("Price");
		label.xalign = 0;
		this.editorGrid.attach_next_to(label, this.price,
			Gtk.PositionType.LEFT, 1, 1);
		this.dp.Add2(this.editorGrid);

		// these two must come before the selected ones so the variables can be set for the initial signal
		this.nameChangedHandler = this.name.changed.connect(() => {
			if (!this.selected)
				GLib.error("item name changed with no item selected");
			this.items.set(this.selection, 0, this.name.text);
		});
		this.priceChangedHandler = this.price.changed.connect(() => {
			if (!this.selected)
				GLib.error("item price changed with no item selected");
			if (this.price.Valid)
				this.items.set(this.selection, 1, this.price.Price);
		});

		this.selected = false;
		this.list.get_selection().changed.connect(() => {
			this.selected = this.list.get_selection().get_selected(null, out this.selection);
			this.name.sensitive = this.selected;
			this.price.sensitive = this.selected;
			this.remove.sensitive = this.selected;
			if (this.selected) {
				string n;
				Price p;

				this.items.get(this.selection, 0, out n, 1, out p);
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

		this.add.clicked.connect(() => {
			Gtk.TreeIter iter;

			this.items.append(out iter);
			this.items.set(iter, 0, "New Item");
			this.list.get_selection().select_iter(iter);
		});
		this.remove.clicked.connect(() => {
			if (!this.selected)
				GLib.error("remove item button clicked without any item selected");
			this.items.remove(this.selection);
		});
	}
}
