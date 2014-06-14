// 14 june 2014
#include "simplesale.h"

// can be shared; thanks mclasen in irc.gimp.net/#gtk+
static GtkListStore *items;

void initItems(void)
{
	items = gtk_list_store_new(2,
		G_TYPE_STRING,			// item name
		G_TYPE_STRING);			// display price
	// index is the item ID
}

void addItem(char *name, char *dispPrice)
{
	GtkTreeIter iter;

	gtk_list_store_append(items, &iter);
	gtk_list_store_set(items, &iter, 0, name, 1, dispPrice, -1);
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
