// 23 march 2015
#include "simplesale.h"

struct ManagerPriv {
};

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

static const char *authors[] = {
	"Pietro Gagliardi",
	NULL,
};

static void aboutClicked(GtkButton *button, gpointer data)
{
	Manager *m = (Manager *) data;
	GtkWidget *a;

	a = gtk_about_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(a), GTK_WINDOW(m->main));
	gtk_window_set_modal(GTK_WINDOW(a), TRUE);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(a), "simplesale");		// TODO
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(a), "A dead-simple point of sale system.");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(a), "Copyright ©2014-2015 Pietro Gagliardi");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(a), authors);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(a), GTK_LICENSE_MIT_X11);
	gtk_dialog_run(GTK_DIALOG(a));
	gtk_widget_destroy(a);
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

	g_signal_connect(m->about, "clicked", G_CALLBACK(aboutClicked), m);

	gtk_widget_show_all(m->main);
	gtk_main();
	gtk_widget_destroy(m->main);
	g_free(m);
	return doQuit;
}
