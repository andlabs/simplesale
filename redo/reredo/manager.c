// 23 march 2015
#include "simplesale.h"

#include "zmanager.h"

static const struct {
	char *icon;
	char *label;
	GtkWidget *(*new)(void);
} pageList[] = {
	// TODO trim down labels?
	{ "insert-object", "Item Editor", newItemEditor },
	{ "contact-new", "Employee Editor", newEmployeeEditor },
	{ "printer", "Device Editor", newDeviceEditor },			// TODO rename to Printers?
	{ "preferences-other", "Other Settings", newOtherSettings },
	{ "list-remove", "Add/Remove Money", newAddRemoveMoney },
	{ "utilities-system-monitor", "Log Viewer", newLogViewer },
	{ NULL, NULL, NULL },
};

static void toLoginClicked(GtkButton *b, gpointer data)
{
	gboolean *doQuit = (gboolean *) data;

	*doQuit = FALSE;
	gtk_main_quit();
}

static void quitClicked(GtkButton *b, gpointer data)
{
	gboolean *doQuit = (gboolean *) data;

	*doQuit = TRUE;
	gtk_main_quit();
}

gboolean manager(void)
{
	Manager *m;
	gboolean doQuit;
	int i;
	GtkWidget *page;

	m = makeManagerFromUIFile();

	g_signal_connect(m->toLogin, "clicked", G_CALLBACK(toLoginClicked), &doQuit);
	g_signal_connect(m->quit, "clicked", G_CALLBACK(quitClicked), &doQuit);

	for (i = 0; pageList[i].icon != NULL; i++) {
		page = (*(pageList[i].new))();
		gtk_stack_add_titled(GTK_STACK(m->pageStack), page, pageList[i].label, pageList[i].label);
	}

	gtk_widget_show_all(m->main);
	gtk_main();
	gtk_widget_destroy(m->main);
	g_free(m);
	return doQuit;
}

int main(void)
{
	gtk_init(NULL, NULL);
	backend = Backend(g_object_new(MockBackendType, NULL));
	PriceEntryType;
	while (!manager())
		;
	return 0;
}
