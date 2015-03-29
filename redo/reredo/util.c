// 29 march 2015
#include "simplesale.h"

void setName0Price1TreeViewColumnLayout(GtkTreeView *tv)
{
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;

	// TODO clear columns
	r = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("Item", r, "text", 0, NULL);
	gtk_tree_view_column_set_expand(col, TRUE);
	gtk_tree_view_append_column(tv, col);
	r = newPriceRenderer();
	col = gtk_tree_view_column_new_with_attributes("Price", r, "price", 1, NULL);
	gtk_tree_view_append_column(tv, col);
	gtk_tree_view_set_headers_visible(tv, TRUE);
}
