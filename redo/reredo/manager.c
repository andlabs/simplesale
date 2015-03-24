// 23 march 2015
#include "simplesale.h"

#include "zmanager.h"

static const struct {
	char *icon;
	char *label;
	GtkWidget *(*new)(void);
} pageList[] = {
	// TODO trim down labels?
	{ "insert-object", "Item Editor", gtk_switch_new },
	{ "contact-new", "Employee Editor", gtk_calendar_new },
	{ "printer", "Device Editor", gtk_color_button_new },	// TODO rename to Printers?
	{ "preferences-other", "Other Options", gtk_header_bar_new },
	{ "list-remove", "Add/Remove Money", gtk_places_sidebar_new },
	{ "utilities-system-monitor", "Log", gtk_hsv_new },
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

	// TODO verify window title

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
	while (!manager())
		;
	return 0;
}
