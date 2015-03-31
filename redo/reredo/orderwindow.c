// 28 march 2015
#include "simplesale.h"

// TODO
// - https://git.gnome.org/browse/gedit/tree/gedit/resources/css/gedit-style.css
// - rename OrderWindowUI to OrderWindowGUI?
// - macroize o->priv->o and o->priv->ow
// - a better way would be to make OrderWindow a subclass of GtkWindow and make o->priv->ow->main the OrderWindow itself but that would probably require some tooling/code generator muckery

struct OrderWindowUIPriv {
};

#include "zorderwindow.h"

struct OrderWindowPriv {
	Order *o;
	OrderWindowUI *ow;
};

G_DEFINE_TYPE(OrderWindow, OrderWindow, G_TYPE_OBJECT)

static void addItem(GtkIconView *iv, GtkTreePath *path, gpointer data)
{
	OrderWindow *o = OrderWindow(data);
	GtkTreeIter iter;

	BackendItemsPathToIter(backend, path, &iter);
	OrderAppendItem(o->priv->o, &iter);
	// TODO update total/subtotal labels here?
}

static void removeItem(GtkToolButton *b, gpointer data)
{
	OrderWindow *o = OrderWindow(data);
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected(o->priv->ow->orderListSelection, NULL, &iter) == FALSE)
		g_error("order window remove item button clicked with no item selected");
	OrderDeleteItem(o->priv->o, &iter);
	// TODO update totals
}

static void OrderWindow_init(OrderWindow *o)
{
	Order *order;
	OrderWindowUI *ow;

	o->priv = G_TYPE_INSTANCE_GET_PRIVATE(o, OrderWindowType, struct OrderWindowPriv);

	order = newOrder();
	ow = makeOrderWindowUIFromUIFile();
	// TODO grow the window

	OrderSetTreeView(order, GTK_TREE_VIEW(ow->orderList));
	BackendSetItemsIconView(backend, GTK_ICON_VIEW(ow->items));

	g_signal_connect(ow->items, "item-activated", G_CALLBACK(addItem), o);
	g_signal_connect(ow->removeItem, "clicked", G_CALLBACK(removeItem), o);

	o->priv->o = order;
	o->priv->ow = ow;
}

static void OrderWindow_dispose(GObject *obj)
{
	G_OBJECT_CLASS(OrderWindow_parent_class)->dispose(obj);

	// TODO free anything here?
}

static void OrderWindow_finalize(GObject *obj)
{
	OrderWindow *o = OrderWindow(obj);

	G_OBJECT_CLASS(OrderWindow_parent_class)->finalize(obj);

	// TODO free these here?
	gtk_widget_destroy(o->priv->ow->main);
	g_free(o->priv->ow);
	deleteOrder(o->priv->o);
}

static void OrderWindow_class_init(OrderWindowClass *class)
{
	g_type_class_add_private(class, sizeof (struct OrderWindowPriv));

	G_OBJECT_CLASS(class)->dispose = OrderWindow_dispose;
	G_OBJECT_CLASS(class)->finalize = OrderWindow_finalize;
}

OrderWindow *newOrderWindow(void)
{
	return OrderWindow(g_object_new(OrderWindowType, NULL));
}

void OrderWindowShow(OrderWindow *o)
{
	gtk_widget_show_all(o->priv->ow->main);
}
