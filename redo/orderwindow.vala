// 6 september 2014

public class OrderWindow : Gtk.Window {
	private DualPane dp;
	private new Gtk.Entry name;
	private Gtk.Button deleteItem;
	private Gtk.Button payNow;
	private Gtk.Button payLater;
	private Gtk.SearchEntry search;
	private Gtk.Button cancel;

	private Gtk.TreeView order;
	private Gtk.ScrolledWindow orderScroller;
	private Gtk.IconView items;
	private Gtk.ScrolledWindow itemsScroller;

	private Order o;

	public OrderWindow(Order o)
	{
		GLib.Object(type: Gtk.WindowType.TOPLEVEL);
		this.title = "simplesale";
		ScaleWindowUp(this, 5, 3);

		this.o = o;

		this.dp = new DualPane(this);
		this.name = new Gtk.Entry();
		this.name.placeholder_text = "Enter customer's name";
		this.dp.LeftHeader.pack_start(this.name);
		this.deleteItem = new Gtk.Button.from_icon_name("list-remove-symbolic", Gtk.IconSize.BUTTON);
		this.dp.LeftHeader.pack_end(this.deleteItem);
		this.payNow = new Gtk.Button.with_label("Pay Now");
		this.payNow.get_style_context().add_class("suggested-action");
		this.dp.RightHeader.pack_start(this.payNow);
		this.payLater = new Gtk.Button.with_label("Pay Later");
		this.payLater.get_style_context().add_class("suggested-action");
		this.dp.RightHeader.pack_start(this.payLater);
		this.search = new Gtk.SearchEntry();
		this.search.placeholder_text = "Find/filter items";
		this.dp.RightHeader.custom_title = this.search;
		this.cancel = new Gtk.Button.with_label("Cancel Order");
		this.cancel.get_style_context().add_class("destructive-action");
		this.dp.RightHeader.pack_end(this.cancel);

		this.order = new Gtk.TreeView();
		this.o.SetupTreeView(this.order);
		this.orderScroller = new Gtk.ScrolledWindow(null, null);
		this.orderScroller.shadow_type = Gtk.ShadowType.IN;
		this.orderScroller.add(this.order);
		this.items = new Gtk.IconView();
		// I hate implicit this
		global::items.SetupIconView(this.items);
		this.items.activate_on_single_click = true;
		this.itemsScroller = new Gtk.ScrolledWindow(null, null);
		this.itemsScroller.shadow_type = Gtk.ShadowType.IN;
		this.itemsScroller.add(this.items);
		this.dp.Add1(this.orderScroller);
		this.dp.Add2(this.itemsScroller);

		// for this one, the this.o being the first object is important; otherwise, continuing a pending order wouldn't work right
		this.o.bind_property("Customer",
			this.name, "text",
			GLib.BindingFlags.DEFAULT | GLib.BindingFlags.BIDIRECTIONAL | GLib.BindingFlags.SYNC_CREATE);
		this.o.bind_property("TotalString",
			this.dp.LeftHeader, "title",
			GLib.BindingFlags.DEFAULT | GLib.BindingFlags.SYNC_CREATE);
		this.o.bind_property("SubtotalString",
			this.dp.LeftHeader, "subtitle",
			GLib.BindingFlags.DEFAULT | GLib.BindingFlags.SYNC_CREATE);

		this.items.item_activated.connect((path) => {
			this.o.Append(path);
		});

		// unfortunately this isn't bindable
		this.order.get_selection().changed.connect(() => {
			this.deleteItem.sensitive = this.order.get_selection().get_selected(null, null);
		});
		// and set initial value
		this.order.get_selection().changed();
		this.deleteItem.clicked.connect(() => {
			Gtk.TreeIter iter;

			if (this.order.get_selection().get_selected(null, out iter) == false)
				GLib.error("delete item button clicked with no item selected (should be disabled)");
			this.o.Delete(iter);
		});

		this.payNow.clicked.connect(() => {
			PayDialog d;
			int response;

			d = new PayDialog(this, 0);	// TODO
			response = d.run();
			d.destroy();
			if (response == Gtk.ResponseType.ACCEPT) {
				this.PayNow(this.o);
				this.destroy();
			}
		});
		this.payLater.clicked.connect(() => {
			// TODO warn of empty customer name
			this.PayLater(this.o);
			this.destroy();
		});
	}

	public signal void PayNow(Order o);
	public signal void PayLater(Order o);
}
