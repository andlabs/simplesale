// 10 september 2014

public class ItemEditor : ManagerTask {
	private DualPane dp;
	private Gtk.SearchEntry search;
	private Gtk.Button add;
	private Gtk.Button remove;
	private Gtk.Button save;
	private Gtk.Button cancel;

	private Gtk.TreeView items;
	private Gtk.ScrolledWindow itemsScroller;

	private Gtk.Grid editorGrid;
	private Gtk.Entry name;
	private PriceEntry price;

	private bool selected;
	private Gtk.TreeIter selection;

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

		this.items = new Gtk.TreeView();
		global::items.SetupTreeView(this.items);
		this.itemsScroller = new Gtk.ScrolledWindow(null, null);
		this.itemsScroller.shadow_type = Gtk.ShadowType.IN;
		this.itemsScroller.add(this.items);
		this.dp.Add1(this.itemsScroller);

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
	}
}
