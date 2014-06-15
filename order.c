// 14 june 2014
#include "simplesale.h"

struct order {
	GtkListStore *store;
	price subtotal;
};

order *newOrder(void)
{
	order *o;

	o = (order *) g_malloc0(sizeof (order));
	o->store = gtk_list_store_new(3,
		G_TYPE_STRING,		// item name
		G_TYPE_STRING,		// display price
		G_TYPE_INT);			// index in items model
	return o;
}

void freeOrder(order *o)
{
	g_object_unref(o->store);
	g_free(o);
}

void addToOrder(order *o, gint index)
{
	char *name, *dispPrice;
	price p;
	GtkTreeIter iter;

	getItem(index, &name, &dispPrice, &p);
	gtk_list_store_append(o->store, &iter);
	gtk_list_store_set(o->store, &iter, 0, name, 1, dispPrice, 2, index, -1);
	o->subtotal += p;
}

void removeFromOrder(order *o, GtkTreeIter *which)
{
	gint index;
	price p;

	gtk_tree_model_get(GTK_TREE_MODEL(o->store), which, 2, &index, -1);
	getItem(index, NULL, NULL, &p);
	gtk_list_store_remove(o->store, which);
	o->subtotal -= p;
}

price subtotal(order *o)
{
	return o->subtotal;
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
