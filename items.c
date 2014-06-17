// 14 june 2014
#include "simplesale.h"

// can be shared; thanks mclasen in irc.gimp.net/#gtk+
static GtkListStore *items;

void initItems(void)
{
	items = gtk_list_store_new(2,
		G_TYPE_STRING,			// item name
		PRICETYPE);				// price
	// index is the item ID
	loadItems();
}

GtkTreeIter addItem(char *name, Price price)
{
	GtkTreeIter iter;

	gtk_list_store_append(items, &iter);
	gtk_list_store_set(items, &iter, 0, name, 1, price, -1);
	return iter;
}

void deleteItem(GtkTreeIter *which)
{
	gtk_list_store_remove(items, which);
}

void getItem(gint index, char **name, Price *p)
{
	GtkTreePath *path;
	GtkTreeIter iter;
	char *a;	// temporary places
	Price c;

	path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(items), &iter, path);
	gtk_tree_model_get(GTK_TREE_MODEL(items), &iter, 0, &a, 1, &c, -1);
	if (name != NULL)
		*name = a;
	if (p != NULL)
		*p = c;
}

GtkListStore *itemsStore(void)
{
	return items;
}

GtkTreeModel *itemsModel(void)
{
	return GTK_TREE_MODEL(items);
}

void setItemsIconLayout(GtkCellLayout *layout)
{
	GtkCellRenderer *r;

	gtk_cell_layout_clear(layout);
	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(layout, r, TRUE);
	gtk_cell_layout_set_attributes(layout, r, "text", 0, NULL);
	r = newPriceRenderer();
	gtk_cell_layout_pack_start(layout, r, FALSE);
	gtk_cell_layout_set_attributes(layout, r, "price", 1, NULL);
}

void setItemsColumnLayout(GtkTreeView *table)
{
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;

	r = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("Item", r, "text", 0, NULL);
	gtk_tree_view_column_set_expand(col, TRUE);
	gtk_tree_view_append_column(table, col);
	r = newPriceRenderer();
	col = gtk_tree_view_column_new_with_attributes("Price", r, "price", 1, NULL);
	gtk_tree_view_append_column(table, col);
	gtk_tree_view_set_headers_visible(table, TRUE);
}

// this is the GUI for editing items

struct ItemEditor {
	GtkWidget *win;
	GtkWidget *layout;
	GtkWidget *leftside;
	GtkWidget *search;
	GtkWidget *remove;
	GtkWidget *list;
	GtkTreeSelection *listSel;		// convenience
	GtkWidget *listScroller;
	GtkWidget *rightside;
	GtkWidget *name;
	GtkWidget *price;
	GtkWidget *invalid;

	GtkTreeIter current;
	gboolean selecting;
};

static void saveClicked(GtkButton *button, gpointer data)
{
	USED(button);

	ItemEditor *e = (ItemEditor *) data;

	saveItems();
	gtk_main_quit();USED(e);// TODO signal completion
}

static void discardClicked(GtkButton *button, gpointer data)
{
	USED(button);

	ItemEditor *e = (ItemEditor *) data;
	GtkWidget *alert;
	gint response;

	alert = gtk_message_dialog_new(GTK_WINDOW(e->win), GTK_DIALOG_MODAL,
		GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
		"Are you sure you want to discard all changes and leave the Item Editor?");
	response = gtk_dialog_run(GTK_DIALOG(alert));
	gtk_widget_destroy(alert);
	if (response != GTK_RESPONSE_YES)
		return;
	g_object_unref(items);
	initItems();
	gtk_main_quit();USED(e);// TODO signal completion
}

static void addItemClicked(GtkButton *button, gpointer data)
{
	USED(button);

	ItemEditor *e = (ItemEditor *) data;
	GtkTreeIter iter;

	iter = addItem("New Item", 0);
	gtk_tree_selection_select_iter(e->listSel, &iter);
}

static void removeItemClicked(GtkButton *button, gpointer data)
{
	USED(button);

	ItemEditor *e = (ItemEditor *) data;
	GtkTreeIter iter;
	char *name;
	GtkWidget *alert;
	gint response;

	if (gtk_tree_selection_get_selected(e->listSel, NULL, &iter) == FALSE)
		g_error("Delete Item button clicked without any item selected (button should be disabled)");
	gtk_tree_model_get(GTK_TREE_MODEL(items), &iter, 0, &name, -1);
	alert = gtk_message_dialog_new(GTK_WINDOW(e->win), GTK_DIALOG_MODAL,
		GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
		"Are you sure you want to delete \"%s\"?", name);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(alert),
		"Once you click Save and Close in the Item Editor window, the item will be permanently deleted and cannot be recovered later.");
	response = gtk_dialog_run(GTK_DIALOG(alert));
	gtk_widget_destroy(alert);
	if (response != GTK_RESPONSE_YES)
		return;
	deleteItem(&iter);
}

static void itemSelected(GtkTreeSelection *selection, gpointer data)
{
	USED(selection);

	ItemEditor *e = (ItemEditor *) data;
	gboolean selected;
	char *name = "";
	char *pricestr = "";
	Price price;

	selected = gtk_tree_selection_get_selected(e->listSel, NULL, &e->current);
	gtk_widget_set_sensitive(e->remove, selected);
	gtk_widget_set_sensitive(e->name, selected);
	gtk_widget_set_sensitive(e->price, selected);
	if (selected) {
		gtk_tree_model_get(GTK_TREE_MODEL(items), &e->current, 0, &name, 1, &price, -1);
		pricestr = priceToString(price, "");
	} else
		gtk_label_set_text(GTK_LABEL(e->invalid), "");
	e->selecting = TRUE;
	gtk_entry_set_text(GTK_ENTRY(e->name), name);
	gtk_entry_set_text(GTK_ENTRY(e->price), pricestr);
	e->selecting = FALSE;
	if (selected)
		g_free(pricestr);

	// informational
	if (selected)
		gtk_entry_set_placeholder_text(GTK_ENTRY(e->name), "Enter item name here and item price below.");
	else
		gtk_entry_set_placeholder_text(GTK_ENTRY(e->name), "Select an item at left or click + to add a new item.");
}

static void nameChanged(GtkEditable *editable, gpointer data)
{
	USED(editable);

	ItemEditor *e = (ItemEditor *) data;
	GtkTreeIter iter;

	if (e->selecting)	// prevent spurious g_error() during selection changed
		return;
	if (gtk_tree_selection_get_selected(e->listSel, NULL, &iter) == FALSE)
		g_error("item changed without any item selected (textbox should be disabled)");
	gtk_list_store_set(items, &iter, 0, gtk_entry_get_text(GTK_ENTRY(e->name)), -1);
}

static void priceChanged(GtkEditable *editable, gpointer data)
{
	USED(editable);

	ItemEditor *e = (ItemEditor *) data;
	GtkTreeIter iter;
	Price price;

	if (e->selecting)	// prevent spurious g_error() during selection changed
		return;
	if (gtk_tree_selection_get_selected(e->listSel, NULL, &iter) == FALSE)
		g_error("item changed without any item selected (textbox should be disabled)");
	if (priceEntryGetPrice(PRICE_ENTRY(e->price), &price) != priceEntryOK)
		gtk_label_set_text(GTK_LABEL(e->invalid), "Entered price invalid; not changing.");
	else {
		gtk_label_set_text(GTK_LABEL(e->invalid), "");
		gtk_list_store_set(items, &iter, 1, price, -1);
	}
}

ItemEditor *newItemEditor(void)
{
	ItemEditor *e;
	GtkWidget *topbar;
	GtkWidget *button;
	GtkWidget *label;

	e = (ItemEditor *) g_malloc0(sizeof (ItemEditor));

	e->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(e->win), "simplesale");
	// TODO remove the following
	g_signal_connect(e->win, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	expandWindowHeight(GTK_WINDOW(e->win), 2);

	topbar = newHeaderBar("Item Editor", e->win);
	newConfirmHeaderButton("Save and Close", G_CALLBACK(saveClicked), e, topbar);
	newCancelHeaderButton("Discard and Close", G_CALLBACK(discardClicked), e, topbar);

	e->layout = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(e->layout), TRUE);

	e->leftside = gtk_grid_new();
	e->search = gtk_search_entry_new();
	// no live search; we assign the interactive search widget of the GtkTreeView below to this
	gtk_widget_set_hexpand(e->search, TRUE);
	gtk_widget_set_halign(e->search, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		e->search, NULL,
		GTK_POS_TOP, 1, 1);
	button = gtk_button_new_from_icon_name("list-add", GTK_ICON_SIZE_BUTTON);
	// TODO style class?
	g_signal_connect(button, "clicked", G_CALLBACK(addItemClicked), e);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		button, e->search,
		GTK_POS_RIGHT, 1, 1);
	e->remove = gtk_button_new_from_icon_name("list-remove", GTK_ICON_SIZE_BUTTON);
	// TODO style class?
	g_signal_connect(e->remove, "clicked", G_CALLBACK(removeItemClicked), e);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		e->remove, button,
		GTK_POS_RIGHT, 1, 1);

	e->list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(items));
	e->listSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(e->list));
	// TODO figure out how to make it so that clicking on blank space deselects
	g_signal_connect(e->listSel, "changed", G_CALLBACK(itemSelected), e);
	setItemsColumnLayout(GTK_TREE_VIEW(e->list));
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(e->list), GTK_ENTRY(e->search));
	e->listScroller = newListScroller(e->list);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		e->listScroller, e->search,
		GTK_POS_BOTTOM, 4, 1);

	e->rightside = gtk_grid_new();
	label = gtk_label_new("Name:");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		label, NULL,
		GTK_POS_TOP, 1, 1);
	e->name = gtk_entry_new();
	g_signal_connect(e->name, "changed", G_CALLBACK(nameChanged), e);
	gtk_widget_set_hexpand(e->name, TRUE);
	gtk_widget_set_halign(e->name, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->name, label,
		GTK_POS_RIGHT, 3, 1);
{GtkWidget *l2 = label;
	label = gtk_label_new("Price:");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		label, l2,
		GTK_POS_BOTTOM, 1, 1);
l2=label;
	label = gtk_label_new("$");
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		label, l2,
		GTK_POS_RIGHT, 1, 1);
}
	e->price = newPriceEntry();
	g_signal_connect(e->price, "changed", G_CALLBACK(priceChanged), e);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->price, label,
		GTK_POS_RIGHT, 1, 1);
	e->invalid = gtk_label_new("");
	alignLabel(e->invalid, 0);
	gtk_widget_set_hexpand(e->invalid, TRUE);
	gtk_widget_set_halign(e->invalid, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->invalid, e->price,
		GTK_POS_RIGHT, 1, 1);

	// TODO add an undo button?

	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->leftside, NULL,
		GTK_POS_TOP, 1, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->rightside, e->leftside,
		GTK_POS_RIGHT, 2, 1);

	gtk_container_add(GTK_CONTAINER(e->win), e->layout);
	gtk_widget_show_all(e->win);

	itemSelected(NULL, e);		// set up initial state

	return e;
}
