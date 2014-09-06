// 6 september 2014

public class OrderEditor : Gtk.Window {
	Gtk.Paned header;
	Gtk.HeaderBar hbleft;
	Gtk.Entry name;
	Gtk.Button deleteItem;
	Gtk.HeaderBar hbright;
	Gtk.Button payNow;
	Gtk.Button payLater;
	Gtk.SearchEntry search;
	Gtk.Button cancel;

	Gtk.Paned body;
	Gtk.TreeView order;
	Gtk.ScrolledWindow orderScroller;
	Gtk.IconView items;
	Gtk.ScrolledWindow itemsScroller;

	public OrderEditor() {
		GLib.Object(type: Gtk.WindowType.TOPLEVEL);
		this.title = "simplesale";
		// TODO get rid fo this
		this.destroy.connect(Gtk.main_quit);

		this.header = new Gtk.Paned(Gtk.Orientation.HORIZONTAL);
		this.hbleft = new Gtk.HeaderBar();
		this.hbleft.show_close_button = false;
		this.name = new Gtk.Entry();
		this.name.placeholder_text = "Enter customer's name";
		this.name.hexpand = true;
		this.name.halign = Gtk.Align.FILL;
		this.hbleft.pack_start(this.name);
		this.deleteItem = new Gtk.Button.from_icon_name("list-remove-symbolic", Gtk.IconSize.BUTTON);
		this.hbleft.pack_end(this.deleteItem);
		this.hbright = new Gtk.HeaderBar();
		this.hbright.show_close_button = false;
		this.payNow = new Gtk.Button.with_label("Pay Now");
		this.payNow.get_style_context().add_class("suggested-action");
		this.hbright.pack_start(this.payNow);
		this.payLater = new Gtk.Button.with_label("Pay Later");
		this.payLater.get_style_context().add_class("suggested-action");
		this.hbright.pack_start(this.payLater);
		this.search = new Gtk.SearchEntry();
		this.search.placeholder_text = "Find/filter items";
		this.hbright.custom_title = this.search;
		this.cancel = new Gtk.Button.with_label("Cancel Order");
		this.cancel.get_style_context().add_class("destructive-action");
		this.hbright.pack_end(this.cancel);
		this.header.add1(this.hbleft);
		this.header.add2(this.hbright);
		this.set_titlebar(this.header);

		this.body = new Gtk.Paned(Gtk.Orientation.HORIZONTAL);
		this.order = new Gtk.TreeView();
		this.orderScroller = new Gtk.ScrolledWindow(null, null);
		this.orderScroller.shadow_type = Gtk.ShadowType.IN;
		this.orderScroller.add(this.order);
		this.items = new Gtk.IconView();
		itemsSetupIconView(this.items);
		this.itemsScroller = new Gtk.ScrolledWindow(null, null);
		this.itemsScroller.shadow_type = Gtk.ShadowType.IN;
		this.itemsScroller.add(this.items);
		this.body.add1(this.orderScroller);
		this.body.add2(this.itemsScroller);

		this.add(this.body);
		this.header.bind_property("position",
			this.body, "position",
			GLib.BindingFlags.DEFAULT | GLib.BindingFlags.BIDIRECTIONAL | GLib.BindingFlags.SYNC_CREATE);
	}
}

public int main(string[] args)
{
	Gtk.init(ref args);
	initItems();
	(new OrderEditor()).show_all();
	Gtk.main();
	return 0;
}
