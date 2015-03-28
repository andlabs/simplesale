// 24 march 2015
#include "simplesale.h"

struct EmployeeEditorPriv {
};

#include "zemployeeeditor.h"

static void destroy(GtkWidget *widget, gpointer data)
{
	EmployeeEditor *e = (EmployeeEditor *) data;

	g_free(e);
}

GtkWidget *newEmployeeEditor(void)
{
	EmployeeEditor *e;

	e = makeEmployeeEditorFromUIFile();
	g_signal_connect(e->main, "destroy", G_CALLBACK(destroy), e);

	// TODO connect search handler

	return e->main;
}
