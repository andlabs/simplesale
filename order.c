// 14 june 2014
#include "simplesale.h"

struct Order {
	GObject parent_instance;

	GtkListStore *store;
	Price subtotal;

	GtkWidget *win;
	GtkWidget *topbar;
	GtkWidget *payNow;
	GtkWidget *payLater;
	GtkWidget *cancel;
	GtkWidget *layout;
	GtkWidget *leftside;
	GtkWidget *customer;
	GtkWidget *order;
	GtkTreeSelection *orderSel;		// convenience
	GtkWidget *orderScroller;
	GtkWidget *delete;
	GtkWidget *rightside;
	GtkWidget *searchBox;
	GtkTreeModel *itemsFiltered;
	GtkWidget *items;
	GtkWidget *itemsScroller;
};

typedef struct OrderClass OrderClass;

struct OrderClass {
	GObjectClass parent_class;
};

G_DEFINE_TYPE(Order, order, G_TYPE_OBJECT)

static void buildOrderGUI(Order *);
static void freeOrderGUI(Order *);

static void order_init(Order *o)
{
	o->store = gtk_list_store_new(2,
		G_TYPE_STRING,		// item name
		PRICETYPE);			// price
	buildOrderGUI(o);
}

static void order_dispose(GObject *obj)
{
	Order *o = (Order *) obj;

	freeOrderGUI(o);
	g_object_unref(o->store);
	G_OBJECT_CLASS(order_parent_class)->dispose(obj);
}

static void order_finalize(GObject *obj)
{
	// call g_free() here if needed
	G_OBJECT_CLASS(order_parent_class)->finalize(obj);
}

static guint orderSignals[1];

static void order_class_init(OrderClass *class)
{
	G_OBJECT_CLASS(class)->dispose = order_dispose;
	G_OBJECT_CLASS(class)->finalize = order_finalize;

	orderSignals[0] = g_signal_new(
		"do", order_get_type(),
		G_SIGNAL_RUN_LAST,
		0,				// no class method slot
		NULL, NULL,		// no accumulator
		NULL,			// no marshaller
		G_TYPE_NONE,		// void do(Order *o, gint code, gpointer data);
		1, G_TYPE_INT);	// only specify the middle parameters; thanks larsu in irc.gimp.net/#gtk+
}

Order *newOrder(void)
{
	return (Order *) g_object_new(order_get_type(), NULL);
}

void freeOrder(Order *o)
{
	g_object_unref(o);
}

static void addToOrder(Order *o, GtkTreeIter *iiter)
{
	char *name;
	Price price;
	GtkTreeIter oiter;

	getItem(iiter, &name, &price);
	gtk_list_store_append(o->store, &oiter);
	gtk_list_store_set(o->store, &oiter, 0, name, 1, price, -1);
	o->subtotal += price;
}

static void removeFromOrder(Order *o, GtkTreeIter *which)
{
	Price price;

	gtk_tree_model_get(GTK_TREE_MODEL(o->store), which, 1, &price, -1);
	gtk_list_store_remove(o->store, which);
	o->subtotal -= price;
}

static Price total(Order *o)
{
	return o->subtotal;
}

// GUI stuff

static void updateTotalDisp(Order *o)
{
	Price tot, sub;
	char *str;

	// TODO move the gut of the work to price.c
	sub = o->subtotal;
	tot = total(o);
	str = priceToString(tot, "Total: $");
	gtk_header_bar_set_title(GTK_HEADER_BAR(o->topbar), str);
	g_free(str);
	str = priceToString(tot, "Subtotal: $");
	gtk_header_bar_set_subtitle(GTK_HEADER_BAR(o->topbar), str);
	g_free(str);
}

static void search(GtkSearchEntry *entry, gpointer data)
{
	USED(entry);

	Order *o = (Order *) data;

	gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(o->itemsFiltered));
}

static gboolean filter(GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
	Order *o = (Order *) data;
	gchar *query;
	gchar *item;
	gchar **words, **wp;

	// if no search, show everything
	// cast to gchar * because we'll be copying it later (wouldn't be necessary if NULL was guaranteed to not be returned or if g_utf8_strdown() was documented as null safe)
	query = (gchar *) gtk_entry_get_text(GTK_ENTRY(o->searchBox));
	if (query == NULL || *query == '\0')		// empty string
		return TRUE;
	query = g_utf8_strdown(query, -1);
	// TODO change to use getItem()?
	gtk_tree_model_get(model, iter, 0, &item, -1);
	item = g_utf8_strdown(item, -1);
	// this make sure any word exists in the item name
	// TODO really this?
	words = g_strsplit(query, " ", 0);
	for (wp = words; *wp != NULL; wp++)
		if (strstr(item, *wp) != NULL) {
			g_strfreev(words);
			return TRUE;
		}
	g_strfreev(words);
	g_free(item);		// frees the lowercase copy
	g_free(query);		// frees the lowercase copy
	return FALSE;
}

static void itemClicked(GtkIconView *view, GtkTreePath *path, gpointer data)
{
	USED(view);

	Order *o = (Order *) data;
	GtkTreeIter iter;

	path = gtk_tree_model_filter_convert_path_to_child_path(GTK_TREE_MODEL_FILTER(o->itemsFiltered), path);
	if (path == NULL)
		g_error("filtered list store path converted to child path is NULL for a valid item inside itemClicked()");
	iter = itemPathToIter(path);
	addToOrder(o, &iter);
	updateTotalDisp(o);
}

static void adjustDeleteEnabled(GtkTreeSelection *selection, gpointer data)
{
	USED(selection);

	Order *o = (Order *) data;

	gtk_widget_set_sensitive(o->delete,
		gtk_tree_selection_count_selected_rows(o->orderSel) != 0);
}

static void deleteClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Order *o = (Order *) data;
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected(o->orderSel, NULL, &iter) == FALSE)
		g_error("Delete Item button clicked without any item selected (button should be disabled)");
	removeFromOrder(o, &iter);
	updateTotalDisp(o);
}

static void cancelClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Order *o = (Order *) data;

	if (!askConfirm(o->win,
		"If you click Yes, the current order will be voided.",
		"Are you sure you want to cancel the current order?"))
		return;
	g_signal_emit(o, orderSignals[0], 0, orderCancel);
}

static void payNowClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Order *o = (Order *) data;
	PayDialog *p;
	Price paid;

	p = newPayDialog(GTK_WINDOW(o->win), total(o));
	do
		if (runPayDialog(p) != GTK_RESPONSE_ACCEPT) {
			freePayDialog(p);
			return;
		}
	while (payDialogAmountPaid(p, &paid) == FALSE);
	freePayDialog(p);
	// TODO save amount paid
	g_signal_emit(o, orderSignals[0], 0, orderPayNow);
}

static void payLaterClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Order *o = (Order *) data;
	const gchar *customer;

	customer = gtk_entry_get_text(GTK_ENTRY(o->customer));
	if (customer == NULL || *customer == '\0') {
		GtkWidget *alert;

		alert = gtk_message_dialog_new(GTK_WINDOW(o->win), GTK_DIALOG_MODAL,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
			"You did not say which customer this order is for.");
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(alert),
			"Please enter the customer's name in the Customer: field and try again.");
		gtk_dialog_run(GTK_DIALOG(alert));
		gtk_widget_destroy(alert);
		return;
	}
	// all good
	g_signal_emit(o, orderSignals[0], 0, orderPayLater);
}

static void buildOrderGUI(Order *o)
{
	GtkWidget *label;

	o->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(o->win), "simplesale");
	// TODO remove the following
	g_signal_connect(o->win, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	expandWindowHeight(GTK_WINDOW(o->win), 3);

	o->topbar = newHeaderBar("<unset>", o->win);
	o->payNow = newConfirmHeaderButton("Pay Now", G_CALLBACK(payNowClicked), o, o->topbar);
	o->payLater = newConfirmHeaderButton("Pay Later", G_CALLBACK(payLaterClicked), o, o->topbar);
	o->cancel = newCancelHeaderButton("Cancel Order", G_CALLBACK(cancelClicked), o, o->topbar);

	o->leftside = gtk_grid_new();

	label = gtk_label_new("Customer:");
	gtk_grid_attach_next_to(GTK_GRID(o->leftside),
		label, NULL,
		GTK_POS_TOP, 1, 1);
	o->customer = gtk_entry_new();
	gtk_widget_set_hexpand(o->customer, TRUE);
	gtk_widget_set_halign(o->customer, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(o->leftside),
		o->customer, label,
		GTK_POS_RIGHT, 1, 1);

	// TODO rename to orderList or something?
	o->order = gtk_tree_view_new_with_model(GTK_TREE_MODEL(o->store));
	// the column layout of items and orders are identical, so just use that one
	setItemsColumnLayout(GTK_TREE_VIEW(o->order));
	o->orderSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(o->order));
	// TODO figure out how to make it so that clicking on blank space deselects
	g_signal_connect(o->orderSel, "changed", G_CALLBACK(adjustDeleteEnabled), o);
	o->orderScroller = newListScroller(o->order);
	gtk_grid_attach_next_to(GTK_GRID(o->leftside),
		o->orderScroller, label,
		GTK_POS_BOTTOM, 2, 1);

	o->delete = gtk_button_new_with_label("Delete Item");
	gtk_widget_set_sensitive(o->delete, FALSE);		// initial state disabled since initial state is nothing selected
	g_signal_connect(o->delete, "clicked", G_CALLBACK(deleteClicked), o);
	gtk_widget_set_hexpand(o->delete, TRUE);
	gtk_widget_set_halign(o->delete, GTK_ALIGN_END);
	gtk_grid_attach_next_to(GTK_GRID(o->leftside),
		o->delete, o->orderScroller,
		GTK_POS_BOTTOM, 2, 1);

	o->rightside = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(o->rightside), TRUE);

	o->searchBox = gtk_search_entry_new();
	g_signal_connect(o->searchBox, "search-changed", G_CALLBACK(search), o);
	gtk_widget_set_hexpand(o->searchBox, TRUE);
	gtk_widget_set_halign(o->searchBox, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(o->rightside),
		o->searchBox, NULL,
		GTK_POS_TOP, 1, 1);

	// TODO rename items to itemsGrid?
	o->itemsFiltered = newItemsFiltered();
	gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(o->itemsFiltered), filter, o, NULL);
	o->items = gtk_icon_view_new_with_model(o->itemsFiltered);
	g_signal_connect(o->items, "item-activated", G_CALLBACK(itemClicked), o);
	gtk_icon_view_set_activate_on_single_click(GTK_ICON_VIEW(o->items), TRUE);
	setItemsIconLayout(GTK_CELL_LAYOUT(o->items));
	o->itemsScroller = newListScroller(o->items);
	gtk_grid_attach_next_to(GTK_GRID(o->rightside),
		o->itemsScroller, o->searchBox,
		GTK_POS_BOTTOM, 2, 1);

	o->layout = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(o->layout), TRUE);
	gtk_grid_attach_next_to(GTK_GRID(o->layout),
		o->leftside, NULL,
		GTK_POS_TOP, 1, 1);
	gtk_grid_attach_next_to(GTK_GRID(o->layout),
		o->rightside, o->leftside,
		GTK_POS_RIGHT, 2, 1);

	updateTotalDisp(o);		// initial state

	gtk_container_add(GTK_CONTAINER(o->win), o->layout);
	gtk_widget_show_all(o->win);
}

static void freeOrderGUI(Order *o)
{
	gtk_widget_destroy(o->win);		// TODO does this destroy subwidgets?
	g_object_unref(o->itemsFiltered);
}

// TODO make this unnecessary
const gchar *orderCustomer(Order *o)
{
	return gtk_entry_get_text(GTK_ENTRY(o->customer));
}

void orderShowWindow(Order *o)
{
	gtk_widget_show_all(o->win);
}

void orderHideWindow(Order *o)
{
	gtk_widget_hide(o->win);
}
