// 14 june 2014
#include "simplesale.h"

struct order {
	GtkListStore *store;
};

order *newOrder(void)
{
	order *o;

	o = g_malloc(sizeof (order));
	o->store = gtk_list_store_new(3,
		G_TYPE_STRING,		// item name
		G_TYPE_STRING,		// display price
		G_TYPE_INT);			// index in items model
	return o;
}

void addToOrder(order *o, gint index)
{
	// TODO split this into a separate function
	char *name, *dispPrice;
	GtkTreePath *path;
	GtkTreeIter iter;

	path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(itemsModel(), &iter, path);
	gtk_tree_model_get(itemsModel(), &iter, 0, &name, 1, &dispPrice, -1);
	gtk_list_store_append(o->store, &iter);
	gtk_list_store_set(o->store, &iter, 0, name, 1, dispPrice, 2, index, -1);
}

GtkTreeModel *orderModel(order *o)
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
