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
	o->store = gtk_list_store_new(3,
		G_TYPE_STRING,		// item name
		G_TYPE_STRING,		// display price
		G_TYPE_INT);			// index in items model
	buildOrderGUI(o);
}

static void order_dispose(GObject *obj)
{
	Order *o = (Order *) obj;

	freeOrderGUI(o);
	g_object_unref(o->store);
	G_OBJECT_CLASS(order_parent_class)->dispose(obj);
}

static void order_finalize(GObject *o)
{
	// call g_free() here if needed
	G_OBJECT_CLASS(order_parent_class)->finalize(o);
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

void addToOrder(Order *o, gint index)
{
	char *name, *dispPrice;
	Price p;
	GtkTreeIter iter;

	getItem(index, &name, &dispPrice, &p);
	gtk_list_store_append(o->store, &iter);
	gtk_list_store_set(o->store, &iter, 0, name, 1, dispPrice, 2, index, -1);
	o->subtotal += p;
}

void removeFromOrder(Order *o, GtkTreeIter *which)
{
	gint index;
	Price p;

	gtk_tree_model_get(GTK_TREE_MODEL(o->store), which, 2, &index, -1);
	getItem(index, NULL, NULL, &p);
	gtk_list_store_remove(o->store, which);
	o->subtotal -= p;
}

Price subtotal(Order *o)
{
	return o->subtotal;
}

GtkTreeModel *orderModel(Order *o)
{
	return GTK_TREE_MODEL(o->store);
}

void setOrderTableLayout(GtkTreeView *table)
{
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;

	r = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("Item", r, "text", 0, NULL);
	gtk_tree_view_column_set_expand(col, TRUE);
	gtk_tree_view_append_column(table, col);
	r = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("Price", r, "text", 1, NULL);
	gtk_tree_view_append_column(table, col);
	gtk_tree_view_set_headers_visible(table, TRUE);
}

// GUI stuff

static void updateTotalDisp(Order *o)
{
	Price tot, sub;
	char *str;

	sub = o->subtotal;
	// TODO this is where total calculation will go;
	tot = sub;
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
	// cast to gchar * because we'll be copying it later
	query = (gchar *) gtk_entry_get_text(GTK_ENTRY(o->searchBox));
	if (query == NULL || *query == '\0')		// empty string
		return TRUE;
	query = g_utf8_strdown(query, -1);
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

	path = gtk_tree_model_filter_convert_path_to_child_path(GTK_TREE_MODEL_FILTER(o->itemsFiltered), path);
	if (path == NULL)
		g_error("filtered list store path converted to child path is NULL for a valid item inside itemClicked()");
	addToOrder(o, listStorePathIndex(path));
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
	GtkWidget *prompt;
	gint response;

	prompt = gtk_message_dialog_new(GTK_WINDOW(o->win), GTK_DIALOG_MODAL,
		GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
		"Are you sure you want to cancel the current order?");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(prompt),
		"If you click Yes, the current order will be voided.");
	response = gtk_dialog_run(GTK_DIALOG(prompt));
	gtk_widget_destroy(prompt);
	if (response != GTK_RESPONSE_YES)
		return;
	g_signal_emit(o, orderSignals[0], 0, orderCancel);
}

static void payNowClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Order *o = (Order *) data;
	PayDialog *p;
	Price paid;

	// TODO total, not subtotal
	p = newPayDialog(GTK_WINDOW(o->win), o->subtotal);
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
	gint width, height;
	GtkWidget *label;

	o->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(o->win), "simplesale");
	// TODO remove the following
	g_signal_connect(o->win, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	gtk_window_get_default_size(GTK_WINDOW(o->win), &width, &height);
	if (height == -1)
		gtk_window_get_size(GTK_WINDOW(o->win), NULL, &height);
	gtk_window_set_default_size(GTK_WINDOW(o->win), width, height * 3);

	o->topbar = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(o->topbar), FALSE);
	gtk_window_set_titlebar(GTK_WINDOW(o->win), o->topbar);

	o->payNow = gtk_button_new_with_label("Pay Now");
	gtk_style_context_add_class(gtk_widget_get_style_context(o->payNow), "suggested-action");
	g_signal_connect(o->payNow, "clicked", G_CALLBACK(payNowClicked), o);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(o->topbar), o->payNow);
	o->payLater = gtk_button_new_with_label("Pay Later");
	gtk_style_context_add_class(gtk_widget_get_style_context(o->payLater), "suggested-action");
	g_signal_connect(o->payLater, "clicked", G_CALLBACK(payLaterClicked), o);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(o->topbar), o->payLater);

	o->cancel = gtk_button_new_with_label("Cancel Order");
	gtk_style_context_add_class(gtk_widget_get_style_context(o->cancel), "destructive-action");
	g_signal_connect(o->cancel, "clicked", G_CALLBACK(cancelClicked), o);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(o->topbar), o->cancel);

	o->layout = gtk_grid_new();

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

	o->order = gtk_tree_view_new_with_model(orderModel(o));
	o->orderSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(o->order));
	// TODO figure out how to make it so that clicking on blank space deselects
	g_signal_connect(o->orderSel, "changed", G_CALLBACK(adjustDeleteEnabled), o);
	setOrderTableLayout(GTK_TREE_VIEW(o->order));
	o->orderScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(o->orderScroller), o->order);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(o->orderScroller), GTK_SHADOW_IN);
	gtk_widget_set_hexpand(o->orderScroller, TRUE);
	gtk_widget_set_halign(o->orderScroller, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(o->orderScroller, TRUE);
	gtk_widget_set_valign(o->orderScroller, GTK_ALIGN_FILL);
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

	o->itemsFiltered = gtk_tree_model_filter_new(itemsModel(), NULL);
	gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(o->itemsFiltered), filter, o, NULL);
	o->items = gtk_icon_view_new_with_model(o->itemsFiltered);
	g_signal_connect(o->items, "item-activated", G_CALLBACK(itemClicked), o);
	gtk_icon_view_set_activate_on_single_click(GTK_ICON_VIEW(o->items), TRUE);
	setItemsIconLayout(GTK_CELL_LAYOUT(o->items));
	o->itemsScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(o->itemsScroller), o->items);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(o->itemsScroller), GTK_SHADOW_IN);
	gtk_widget_set_hexpand(o->itemsScroller, TRUE);
	gtk_widget_set_halign(o->itemsScroller, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(o->itemsScroller, TRUE);
	gtk_widget_set_valign(o->itemsScroller, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(o->rightside),
		o->itemsScroller, o->searchBox,
		GTK_POS_BOTTOM, 2, 1);

	gtk_grid_set_column_homogeneous(GTK_GRID(o->layout), TRUE);
	gtk_widget_set_hexpand(o->leftside, TRUE);
	gtk_widget_set_halign(o->leftside, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(o->leftside, TRUE);
	gtk_widget_set_valign(o->leftside, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(o->layout),
		o->leftside, NULL,
		GTK_POS_TOP, 1, 1);
	gtk_widget_set_hexpand(o->rightside, TRUE);
	gtk_widget_set_halign(o->rightside, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(o->rightside, TRUE);
	gtk_widget_set_valign(o->rightside, GTK_ALIGN_FILL);
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
