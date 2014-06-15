// 14 june 2014
#include "simplesale.h"

gint listStoreIterIndex(GtkTreeModel *model, GtkTreeIter *iter)
{
	GtkTreePath *path;
	gint index;

	path = gtk_tree_model_get_path(model, iter);
	index = listStorePathIndex(path);
	gtk_tree_path_free(path);
	return index;
}

gint listStorePathIndex(GtkTreePath *path)
{
	gint *indices;

	if (gtk_tree_path_get_depth(path) != 1)
		g_error("list store path indices array has unexpected length: expected 1, got %d",
			gtk_tree_path_get_depth(path));
	indices = gtk_tree_path_get_indices(path);
	if (indices == NULL)
		g_error("list store path indices array is NULL despite having length 1");
	return indices[0];
}

gchar *priceToString(Price p, char *prefix)
{
	Price dollars, cents;

	dollars = p / 100;
	cents = p % 100;
	return g_strdup_printf("%s%" PRICEFMT ".%02" PRICEFMT, prefix, dollars, cents);
}

void alignLabel(GtkWidget *label, gfloat alignment)
{
	gfloat yalign;

	gtk_misc_get_alignment(GTK_MISC(label), NULL, &yalign);
	gtk_misc_set_alignment(GTK_MISC(label), alignment, yalign);
}
