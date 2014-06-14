// 14 june 2014
#include "simplesale.h"

// can be shared; thanks mclasen in irc.gimp.net/#gtk+
static GtkListStore *items;

void initItems(void)
{
	items = gtk_list_store_new(3,
		G_TYPE_STRING,			// item name
		G_TYPE_STRING,			// display price
		G_TYPE_UINT64);			// actual price
	// index is the item ID
}

void addItem(char *name, price p)
{
	GtkTreeIter iter;
	char *dispPrice;

	dispPrice = priceToString(p, "$");
	gtk_list_store_append(items, &iter);
	gtk_list_store_set(items, &iter, 0, name, 1, dispPrice, 2, p, -1);
	g_free(dispPrice);			// the GtkListStore made a copy
}

void getItem(gint index, char **name, char **dispPrice, price *p)
{
	GtkTreePath *path;
	GtkTreeIter iter;
	char *a, *b;	// temporary places
	price c;

	path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(items), &iter, path);
	gtk_tree_model_get(GTK_TREE_MODEL(items), &iter, 0, &a, 1, &b, 2, &c, -1);
	if (name != NULL)
		*name = a;
	if (dispPrice != NULL)
		*dispPrice = b;
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
	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(layout, r, FALSE);
	gtk_cell_layout_set_attributes(layout, r, "text", 1, NULL);
}
