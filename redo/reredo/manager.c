// 23 march 2015
#include "simplesale.h"

typedef struct ManagerPrivate ManagerPrivate;

struct ManagerPrivate {
	Order *o;
	GtkWidget *pageSwitcher;
	GtkWidget *toLogin;
	GtkWidget *quit;
	GtkWidget *pageStack;
	GtkWidget *about;
};

G_DEFINE_TYPE_WITH_CODE(Manager, Manager, GTK_TYPE_WINDOW,
	G_ADD_PRIVATE(Manager))

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
	Manager *m = Manager(data);

	gtk_main_quit();
}

static void quitClicked(GtkButton *b, gpointer data)
{
	Manager *m = Manager(data);

	gtk_main_quit();
}

static const char *authors[] = {
	"Pietro Gagliardi",
	NULL,
};

static void aboutClicked(GtkButton *button, gpointer data)
{
	Manager *m = Manager(data);
	GtkWidget *a;

	a = gtk_about_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(a), GTK_WINDOW(m));
	gtk_window_set_modal(GTK_WINDOW(a), TRUE);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(a), "simplesale");		// TODO
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(a), "A dead-simple point of sale system.");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(a), "Copyright ©2014-2015 Pietro Gagliardi");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(a), authors);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(a), GTK_LICENSE_MIT_X11);
	gtk_dialog_run(GTK_DIALOG(a));
	gtk_widget_destroy(a);
}

static void Manager_init(Manager *m)
{
	int i;
	GtkWidget *page;

	m->priv = Manager_get_instance_private(m);

	gtk_widget_init_template(GTK_WIDGET(m));
	// TODO grow the window

	g_signal_connect(m->priv->toLogin, "clicked", G_CALLBACK(toLoginClicked), m);
	g_signal_connect(m->priv->quit, "clicked", G_CALLBACK(quitClicked), m);

	for (i = 0; pageList[i].icon != NULL; i++) {
		page = (*(pageList[i].new))();
		gtk_stack_add_titled(GTK_STACK(m->priv->pageStack), page, pageList[i].label, pageList[i].label);
	}

	g_signal_connect(m->priv->about, "clicked", G_CALLBACK(aboutClicked), m);
}

static void Manager_dispose(GObject *obj)
{
	G_OBJECT_CLASS(Manager_parent_class)->dispose(obj);

	// TODO free anything here?
}

static void Manager_finalize(GObject *obj)
{
	Manager *o = Manager(obj);

	G_OBJECT_CLASS(Manager_parent_class)->finalize(obj);

	// TODO free this here?
	deleteOrder(o->priv->o);
}

static void Manager_class_init(ManagerClass *class)
{
	G_OBJECT_CLASS(class)->dispose = Manager_dispose;
	G_OBJECT_CLASS(class)->finalize = Manager_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/manager.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Manager, name)
	T(pageSwitcher);
	T(toLogin);
	T(quit);
	T(pageStack);
	T(about);
#undef T
}

// TODO return GtkWidget instead?
Manager *newManager(void)
{
	return Manager(g_object_new(ManagerType, NULL));
}
