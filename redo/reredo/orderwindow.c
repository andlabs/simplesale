// 28 march 2015
#include "simplesale.h"

// TODO
// - https://git.gnome.org/browse/gedit/tree/gedit/resources/css/gedit-style.css
// - rename OrderWindowUI to OrderWindowGUI?
// - macroize o->priv->o and o->priv->ow
// - a better way would be to make OrderWindow a subclass of GtkWindow and make o->priv->ow->main the OrderWindow itself but that would probably require some tooling/code generator muckery
// - don't let empty notes pass through
// - make sure the problem with the item list removal doesn't happen here

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

static void addNote(GtkToolButton *b, gpointer data)
{
	OrderWindow *o = OrderWindow(data);
	GtkWidget *message;
	GtkWidget *note;
	GtkWidget *msgarea;

	message = gtk_message_dialog_new(GTK_WINDOW(o->priv->ow->main), GTK_DIALOG_MODAL,
		// TODO really GTK_MESSAGE_QUESTION?
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
		"Enter the note to add to the order below.");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(message),
		"It must fit on the receipt, so keep it short.");
	note = gtk_entry_new();
	gtk_widget_set_hexpand(note, TRUE);
	gtk_widget_set_halign(note, GTK_ALIGN_FILL);
	msgarea = gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(message));
	gtk_container_add(GTK_CONTAINER(msgarea), note);
	gtk_widget_show_all(note);		// not shown by default by message dialog
	if (gtk_dialog_run(GTK_DIALOG(message)) == GTK_RESPONSE_OK)
		OrderAppendNote(o->priv->o, gtk_entry_get_text(GTK_ENTRY(note)));
	gtk_widget_destroy(message);
}

static void orderListSelected(GtkTreeSelection *sel, gpointer data)
{
	OrderWindow *o = OrderWindow(data);
	gboolean selected;

	selected = gtk_tree_selection_get_selected(o->priv->ow->orderListSelection, NULL, NULL);
	gtk_widget_set_sensitive(o->priv->ow->removeItem, selected);
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
	g_signal_connect(ow->addNote, "clicked", G_CALLBACK(addNote), o);
	g_signal_connect(ow->orderListSelection, "changed", G_CALLBACK(orderListSelected), o);

	o->priv->o = order;
	o->priv->ow = ow;

	// TODO necessary?
	orderListSelected(ow->orderListSelection, o);
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
