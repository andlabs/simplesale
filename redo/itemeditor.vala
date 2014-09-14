// 10 september 2014

public class ItemEditor : ManagerTask {
	private Gtk.HeaderBar hb;

	private Gtk.Grid layout;

	private Gtk.Grid listGrid;
	private Gtk.TreeView list;
	private Gtk.ScrolledWindow listScroller;
	private Gtk.Toolbar listToolbar;
	private Gtk.ToolButton addButton;
	private Gtk.ToolButton removeButton;
	private Gtk.SearchEntry search;

	private new Gtk.Entry name;
	private PriceEntry price;

	private bool selected;
	private Gtk.TreeIter selection;

	private ulong nameChangedHandler;
	private ulong priceChangedHandler;

	// we can't use a normal constructor here because Vala constructor != GObject constructor
	// thanks to Lethalman, flo, and mezen in irc.gimp.net/#vala
	construct {
		Gtk.SeparatorToolItem spring;
		Gtk.ToolItem searchItem;
		Gtk.Label label;

		this.title = "simplesale";
		// TODO get rid of this
		this.destroy.connect(Gtk.main_quit);
		ScaleWindowUp(this, 2, 3);

		this.hb = new Gtk.HeaderBar();
		this.hb.show_close_button = true;
		this.hb.title = "Item Editor";
		this.set_titlebar(this.hb);

		this.layout = new Gtk.Grid();
		this.layout.border_width = 12;
		this.layout.row_spacing = 6;
		this.layout.column_spacing = 12;

		this.listGrid = new Gtk.Grid();

		this.list = new Gtk.TreeView();
		items.SetupTreeView(this.list);
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);
		this.listScroller.hexpand = true;
		this.listScroller.halign = Gtk.Align.FILL;
		this.listScroller.vexpand = true;
		this.listScroller.valign = Gtk.Align.FILL;
		this.listGrid.attach_next_to(this.listScroller, null,
			Gtk.PositionType.BOTTOM, 1, 1);

		this.listToolbar = new Gtk.Toolbar();
		this.listToolbar.toolbar_style = Gtk.ToolbarStyle.ICONS;
		this.listToolbar.get_style_context().add_class(Gtk.STYLE_CLASS_INLINE_TOOLBAR);
		this.listToolbar.icon_size = Gtk.IconSize.MENU;

		this.addButton = new Gtk.ToolButton(null, null);
		this.addButton.icon_name = "list-add-symbolic";
		this.addButton.set_expand(false);
		this.addButton.set_homogeneous(true);
		this.listToolbar.insert(this.addButton, 0);
		this.removeButton = new Gtk.ToolButton(null, null);
		this.removeButton.icon_name = "list-remove-symbolic";
		this.removeButton.set_expand(false);
		this.removeButton.set_homogeneous(true);
		this.listToolbar.insert(this.removeButton, 1);

		spring = new Gtk.SeparatorToolItem();
		spring.set_expand(true);
		spring.set_homogeneous(false);
		spring.set_draw(false);
		this.listToolbar.insert(spring, -1);
		this.search = new Gtk.SearchEntry();
		this.search.placeholder_text = "Find existing item";
		this.list.set_search_entry(this.search);
		searchItem = new Gtk.ToolItem();
		searchItem.add(this.search);
		searchItem.set_expand(false);
		searchItem.set_homogeneous(false);
		this.listToolbar.insert(searchItem, -1);

		this.listToolbar.hexpand = true;
		this.listToolbar.halign = Gtk.Align.FILL;
		this.listGrid.attach_next_to(this.listToolbar, this.listScroller,
			Gtk.PositionType.BOTTOM, 1, 1);

		this.listGrid.hexpand = true;
		this.listGrid.halign = Gtk.Align.FILL;
		this.listGrid.vexpand = true;
		this.listGrid.valign = Gtk.Align.FILL;
		this.layout.attach_next_to(this.listGrid, null,
			Gtk.PositionType.BOTTOM, 2, 1);

		label = new Gtk.Label("Name");
		label.xalign = 0;
		this.layout.attach_next_to(label, this.listGrid,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.name = new Gtk.Entry();
		this.name.hexpand = true;
		this.name.halign = Gtk.Align.FILL;
		this.layout.attach_next_to(this.name, label,
			Gtk.PositionType.RIGHT, 1, 1);
		this.price = new PriceEntry();
		this.price.hexpand = true;
		this.price.halign = Gtk.Align.FILL;
		this.layout.attach_next_to(this.price, this.name,
			Gtk.PositionType.BOTTOM, 1, 1);
		label = new Gtk.Label("Price");
		label.xalign = 0;
		this.layout.attach_next_to(label, this.price,
			Gtk.PositionType.LEFT, 1, 1);

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

		this.add(this.layout);
	}
}
