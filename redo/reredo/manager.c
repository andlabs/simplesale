// 23 march 2015
#include "simplesale.h"

#include "zmanager.h"

static GtkListStore *pages;
static struct {
	char *icon;
	char *label;
	GtkWidget (*new)(void);
} pageList[] = {
	// TODO trim down labels?
	{ "insert-object", "Item Editor", NULL },
	{ "contact-new", "Employee Editor", NULL },
	{ "printer", "Device Editor", NULL },	// TODO rename to Printers?
	{ "preferences-other", "Other Options", NULL },
	{ "list-remove", "Add/Remove Money", NULL },
	{ "utilities-system-monitor", "Log", NULL },
	{ NULL, NULL, NULL },
};

void initManager(void)
{
	int i;
	GtkTreeIter iter;

	pages = gtk_list_store_new(2,
		G_TYPE_STRING,		// icon name
		G_TYPE_STRING);		// label
	for (i = 0; pageList[i].icon != NULL; i++) {
		gtk_list_store_append(pages, &iter);
		gtk_list_store_set(pages, &iter, 0, pageList[i].icon, 1, pageList[i].label, -1);
	}
	// keep pages around so it isn't destroyed after the first Manager window is dismissed
	// TODO necessary?
	g_object_ref(pages);
}

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
	GtkTreePath *firstItem;

	// TODO verify window title

	m = makeManagerFromUIFile();

	gtk_icon_view_set_model(GTK_ICON_VIEW(m->pageSwitcher), GTK_TREE_MODEL(pages));
	// TODO figure this out
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(m->pageSwitcher), 0);
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(m->pageSwitcher), 1);

	g_signal_connect(m->toLogin, "clicked", G_CALLBACK(toLoginClicked), &doQuit);
	g_signal_connect(m->quit, "clicked", G_CALLBACK(quitClicked), &doQuit);

	// and select the first option and get the ball rolling
	firstItem = gtk_tree_path_new_first();
	gtk_icon_view_select_path(GTK_ICON_VIEW(m->pageSwitcher), firstItem);
	gtk_tree_path_free(firstItem);
	gtk_widget_show_all(m->main);
	gtk_main();
	gtk_widget_destroy(m->main);
	g_free(m);
	return doQuit;
}

int main(void)
{
	gtk_init(NULL, NULL);
	initManager();
	while (!manager())
		;
	return 0;
}
