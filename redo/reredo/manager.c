// 23 march 2015
#include "simplesale.h"

struct Manager {
	GtkWidget *window;
	GtkWidget *pageSwitcher;
	GtkWidget *pageStack;
};

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
	struct Manager m;
	GtkWidget *c1, *c2, *c3;
	GtkWidget *pageScroller;
	GtkWidget *toLogin, *quit;
	gboolean doQuit;
	GtkTreePath *firstItem;

	m.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// TODO verify title
	gtk_window_set_title(GTK_WINDOW(m.window), "Manager");
	gtk_container_set_border_width(GTK_CONTAINER(m.window), gtkMargin);

	c1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, gtkXPadding);
	gtk_container_add(GTK_CONTAINER(m.window), c1);

	c2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, gtkYPadding);
	gtk_container_add(GTK_CONTAINER(c1), c2);

	pageScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(pageScroller), GTK_SHADOW_IN);
	gtk_widget_set_vexpand(pageScroller, TRUE);
	gtk_widget_set_valign(pageScroller, GTK_ALIGN_FILL);
	gtk_container_add(GTK_CONTAINER(c2), pageScroller);

	m.pageSwitcher = gtk_icon_view_new_with_model(GTK_TREE_MODEL(pages));
	// TODO figure this out
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(m.pageSwitcher), 0);
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(m.pageSwitcher), 1);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(m.pageSwitcher), 1);
	gtk_icon_view_set_activate_on_single_click(GTK_ICON_VIEW(m.pageSwitcher), TRUE);
	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(m.pageSwitcher), GTK_SELECTION_BROWSE);
	gtk_container_add(GTK_CONTAINER(pageScroller), m.pageSwitcher);

	c3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(c3), TRUE);
	gtk_container_add(GTK_CONTAINER(c2), c3);

	toLogin = gtk_button_new_with_label("To Login");
	g_signal_connect(toLogin, "clicked", G_CALLBACK(toLoginClicked), &doQuit);
	gtk_container_add(GTK_CONTAINER(c3), toLogin);

	quit = gtk_button_new_with_label("Quit");
	g_signal_connect(quit, "clicked", G_CALLBACK(quitClicked), &doQuit);
	gtk_container_add(GTK_CONTAINER(c3), quit);

	m.pageStack = gtk_stack_new();
	gtk_widget_set_hexpand(m.pageStack, TRUE);
	gtk_widget_set_halign(m.pageStack, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(m.pageStack, TRUE);
	gtk_widget_set_valign(m.pageStack, GTK_ALIGN_FILL);
	gtk_container_add(GTK_CONTAINER(c1), m.pageStack);

	// and select the first option and get the ball rolling
	firstItem = gtk_tree_path_new_first();
	gtk_icon_view_select_path(GTK_ICON_VIEW(m.pageSwitcher), firstItem);
	gtk_tree_path_free(firstItem);
	gtk_widget_show_all(m.window);
	gtk_main();
	gtk_widget_destroy(m.window);
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
