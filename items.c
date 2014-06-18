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
