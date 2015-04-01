// 28 march 2015
#include "simplesale.h"

// TODO
// - don't let empty notes pass through
// - make sure the problem with the item list removal doesn't happen here

typedef struct OrderWindowPrivate OrderWindowPrivate;

struct OrderWindowPrivate {
	Order *o;
	GtkWidget *orderList;
	GtkTreeSelection *orderListSelection;
	GtkWidget *removeItem;
	GtkWidget *addNote;
	GtkWidget *search;
	GtkWidget *items;
	GtkWidget *customerName;
	GtkWidget *tableButton;
	GtkWidget *cancel;
	GtkWidget *payLater;
	GtkWidget *payNow;
};

G_DEFINE_TYPE_WITH_CODE(OrderWindow, OrderWindow, GTK_TYPE_WINDOW,
	G_ADD_PRIVATE(OrderWindow))

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

	if (gtk_tree_selection_get_selected(o->priv->orderListSelection, NULL, &iter) == FALSE)
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

	message = gtk_message_dialog_new(GTK_WINDOW(o), GTK_DIALOG_MODAL,
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

	selected = gtk_tree_selection_get_selected(o->priv->orderListSelection, NULL, NULL);
	gtk_widget_set_sensitive(o->priv->removeItem, selected);
}

static void OrderWindow_init(OrderWindow *o)
{
	o->priv = OrderWindow_get_instance_private(o);

	gtk_widget_init_template(GTK_WIDGET(o));
	// TODO grow the window

	o->priv->o = newOrder();

	OrderSetTreeView(o->priv->o, GTK_TREE_VIEW(o->priv->orderList));
	BackendSetItemsIconView(backend, GTK_ICON_VIEW(o->priv->items));

	g_signal_connect(o->priv->items, "item-activated", G_CALLBACK(addItem), o);
	g_signal_connect(o->priv->removeItem, "clicked", G_CALLBACK(removeItem), o);
	g_signal_connect(o->priv->addNote, "clicked", G_CALLBACK(addNote), o);
	g_signal_connect(o->priv->orderListSelection, "changed", G_CALLBACK(orderListSelected), o);

	// TODO necessary?
	orderListSelected(o->priv->orderListSelection, o);
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

	// TODO free this here?
	deleteOrder(o->priv->o);
}

static void OrderWindow_class_init(OrderWindowClass *class)
{
	G_OBJECT_CLASS(class)->dispose = OrderWindow_dispose;
	G_OBJECT_CLASS(class)->finalize = OrderWindow_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/orderwindow.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), OrderWindow, name)
	T(orderList);
	T(orderListSelection);
	T(removeItem);
	T(addNote);
	T(search);
	T(items);
	T(customerName);
	T(tableButton);
	T(cancel);
	T(payLater);
	T(payNow);
#undef T
}

// TODO return GtkWidget instead?
OrderWindow *newOrderWindow(void)
{
	return OrderWindow(g_object_new(OrderWindowType, NULL));
}
