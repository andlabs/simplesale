// 13 june 2014
#include "simplesale.h"

struct orderWindow {
	GtkWidget *win;
	GtkWidget *topbar;
	GtkWidget *payNow;
	GtkWidget *payLater;
	GtkWidget *cancel;
	GtkWidget *layout;
	GtkWidget *leftside;
	GtkWidget *customer;
	order *o;
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

static void search(GtkSearchEntry *entry, gpointer data)
{
	orderWindow *o = (orderWindow *) data;

	gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(o->itemsFiltered));
}

static gboolean filter(GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
	orderWindow *o = (orderWindow *) data;
	gchar *query;
	const gchar *item;
	gchar **words, **wp;

	// if no search, show everything
	query = gtk_entry_get_text(GTK_ENTRY(o->searchBox));
	if (query == NULL || strlen(query) == 0)
		return TRUE;
	gtk_tree_model_get(model, iter, 0, &item, -1);
	// this make sure any word exists in the item name
	// TODO really this?
	words = g_strsplit(query, " ", 0);
	for (wp = words; *wp != NULL; wp++)
		/* TODO is strcasestr() guaranteed available with GLib? */
		if (strcasestr(item, *wp) != NULL) {
			g_strfreev(words);
			return TRUE;
		}
	g_strfreev(words);
	return FALSE;
}

static void itemClicked(GtkIconView *view, GtkTreePath *path, gpointer data)
{
	orderWindow *o = (orderWindow *) data;

	path = gtk_tree_model_filter_convert_path_to_child_path(GTK_TREE_MODEL_FILTER(o->itemsFiltered), path);
	if (path == NULL)
		g_error("filtered list store path converted to child path is NULL for a valid item inside itemClicked()");
	addToOrder(o->o, listStorePathIndex(itemsModel(), path));
}

static void adjustDeleteEnabled(GtkTreeSelection *selection, gpointer data)
{
	orderWindow *o = (orderWindow *) data;

	gtk_widget_set_sensitive(o->delete,
		gtk_tree_selection_count_selected_rows(o->orderSel) != 0);
}

static void deleteClicked(GtkButton *button, gpointer data)
{
	orderWindow *o = (orderWindow *) data;
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected(o->orderSel, NULL, &iter) == FALSE)
		g_error("Delete Item button clicked without any item selected (button should be disabled)");
	removeFromOrder(o->o, &iter);
}

orderWindow *newOrderWindow(void) {
	orderWindow *o;
	gint width, height;
	GtkWidget *label;
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;

	o = g_malloc(sizeof (orderWindow));

	o->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(o->win), "simplesale");
	g_signal_connect(o->win, "delete-event", gtk_main_quit, NULL);

	/* the initail height is too small */
	gtk_window_get_default_size(GTK_WINDOW(o->win), &width, &height);
	if (height == -1)
		gtk_window_get_size(GTK_WINDOW(o->win), NULL, &height);
	gtk_window_set_default_size(GTK_WINDOW(o->win), width, height * 3);

	o->topbar = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(o->topbar), "Total: $3.45");
	gtk_header_bar_set_subtitle(GTK_HEADER_BAR(o->topbar), "Subtotal: $2.34");
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(o->topbar), FALSE);
	gtk_window_set_titlebar(GTK_WINDOW(o->win), o->topbar);

	o->payNow = gtk_button_new_with_label("Pay Now");
	gtk_style_context_add_class(gtk_widget_get_style_context(o->payNow), "suggested-action");
	gtk_header_bar_pack_start(GTK_HEADER_BAR(o->topbar), o->payNow);
	o->payLater = gtk_button_new_with_label("Pay Later");
	gtk_style_context_add_class(gtk_widget_get_style_context(o->payLater), "suggested-action");
	gtk_header_bar_pack_start(GTK_HEADER_BAR(o->topbar), o->payLater);

	o->cancel = gtk_button_new_with_label("Cancel Order");
	gtk_style_context_add_class(gtk_widget_get_style_context(o->cancel), "destructive-action");
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

	o->o = newOrder();
	o->order = gtk_tree_view_new_with_model(orderModel(o->o));
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

	initItems();
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

	/* sample items */
	{
		addItem("Regular Slice", PRICE(2, 00));
		addItem("Large Soda", PRICE(1, 50));
		addItem("Cookie", PRICE(1, 00));
		addToOrder(o->o, 0);
		addToOrder(o->o, 1);
	}

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

	gtk_container_add(GTK_CONTAINER(o->win), o->layout);
	gtk_widget_show_all(o->win);

	return o;
}