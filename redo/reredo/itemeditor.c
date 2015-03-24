// 24 march 2015
#include "simplesale.h"

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

	// TODO connect search handler

	return e->main;
}
