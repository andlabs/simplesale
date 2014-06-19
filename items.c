// 14 june 2014
#include "simplesale.h"

// can be shared; thanks mclasen in irc.gimp.net/#gtk+
static GtkListStore *items = NULL;

void initItems(void)
{
	if (items != NULL)				// free old
		g_object_unref(items);
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

void getItem(GtkTreeIter *iter, char **name, Price *price)
{
	char *a;	// temporary places
	Price c;

	gtk_tree_model_get(GTK_TREE_MODEL(items), iter, 0, &a, 1, &c, -1);
	if (name != NULL)
		*name = a;
	if (price != NULL)
		*price = c;
}

GtkTreeIter itemPathToIter(GtkTreePath *path)
{
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter(GTK_TREE_MODEL(items), &iter, path) == FALSE)
		g_error("error converting items path to iter");
	return iter;
}

void setItemName(GtkTreeIter *which, const char *name)
{
	gtk_list_store_set(items, which, 0, name, -1);
}

void setItemPrice(GtkTreeIter *which, Price price)
{
	gtk_list_store_set(items, which, 1, price, -1);
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

void setItemsModel(GtkTreeView *table)
{
	gtk_tree_view_set_model(table, GTK_TREE_MODEL(items));
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

void loadItems(void)
{
	dbIn *i;
	char *name;
	Price price;

	i = dbInOpenItems();
	while (dbInReadItem(i, &name, &price) == TRUE) {
		addItem(name, price);
		g_free(name);
	}
	dbInCloseAndFree(i);
}

void saveItems(void)
{
	dbOut *o;

	o = dbOutOpenItems();
	dbOutWriteItemModel(GTK_TREE_MODEL(items), o);
	dbOutCreateAndFree(o);
}
