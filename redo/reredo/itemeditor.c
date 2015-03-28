// 24 march 2015
#include "simplesale.h"

struct ItemEditorPriv {
	gboolean selected;
	GtkTreeIter selectedIter;
};

#include "zitemeditor.h"

static void destroy(GtkWidget *widget, gpointer data)
{
	ItemEditor *e = (ItemEditor *) data;

	g_free(e);
}

GtkWidget *newItemEditor(void)
{
	ItemEditor *e;

	e = makeItemEditorFromUIFile();
	g_signal_connect(e->main, "destroy", G_CALLBACK(destroy), e);

	BackendSetItemsTreeView(backend, GTK_TREE_VIEW(e->list));
	// TODO search entry (in .ui file?) and signals

	return e->main;
}
