// 21 june 2014
#include "simplesale.h"

struct Manager {
	GtkWidget *win;
};

static void returnToManager(ManagerEditor *obj, gpointer data)
{
	Manager *m = (Manager *) data;

	gtk_widget_destroy(GTK_WIDGET(obj));
	gtk_widget_show_all(m->win);
}

#define EDITORCLICKED(name, InitName) \
	static void name ## Clicked(GtkButton *button, gpointer data) \
	{ \
		USED(button); \
		Manager *m = (Manager *) data; \
		GtkWidget *e; \
		gtk_widget_hide(m->win); \
		e = new ## InitName(); \
		g_signal_connect(e, "done", G_CALLBACK(returnToManager), m); \
		gtk_widget_show_all(e); \
	}

EDITORCLICKED(itemEditor, ItemEditor)
EDITORCLICKED(accountEditor, AccountEditor)

Manager *newManager(void)
{
	Manager *m;
	GtkWidget *layout;
	GtkWidget *button;

	m = (Manager *) g_malloc0(sizeof (Manager));

	m->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(m->win), "simplesale");
	// TODO remove the following
	g_signal_connect(m->win, "delete-event", gtk_main_quit, NULL);

	newHeaderBar("Manager Control Panel", m->win);

	layout = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(layout), TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(layout), TRUE);

#define B(icon) button = gtk_button_new_from_icon_name(icon, GTK_ICON_SIZE_DIALOG)
#define L(label) gtk_button_set_label(GTK_BUTTON(button), label); gtk_button_set_image_position(GTK_BUTTON(button), GTK_POS_TOP); gtk_button_set_always_show_image(GTK_BUTTON(button), TRUE)
#define C(sig) g_signal_connect(button, "clicked", G_CALLBACK(sig), m)
#define A(x, y) gtk_grid_attach(GTK_GRID(layout), button, y, x, 1, 1)
#define BUTTON(icon, label, sig, x, y) B(icon); L(label); C(sig); A(x, y)

	BUTTON("insert-object", "Item Editor", itemEditorClicked, 0, 0);
	BUTTON("contact-new", "Account Editor", accountEditorClicked, 0, 1);
	BUTTON("printer", "Device Editor", gtk_main_quit, 0, 2);
	BUTTON("preferences-other", "Other Settings", gtk_main_quit, 0, 3);
	BUTTON("list-remove", "Withdraw Money", gtk_main_quit, 1, 0);
	BUTTON("utilities-system-monitor", "View Log", gtk_main_quit, 1, 1);
	BUTTON("system-log-out", "Return to Login Screen", gtk_main_quit, 1, 2);
	BUTTON("application-exit", "Quit simplesale", gtk_main_quit, 1, 3);

	gtk_container_add(GTK_CONTAINER(m->win), layout);
	gtk_widget_show_all(m->win);

	return m;
}

// this is the shared class that the other Manager editors derive from
// it solely exists to provide the "done" signal

G_DEFINE_TYPE(ManagerEditor, managerEditor, GTK_TYPE_WINDOW)

static void managerEditor_init(ManagerEditor *e)
{
	USED(e);

	// do nothing here
}

static void managerEditor_dispose(GObject *obj)
{
	// no need to explicitly gtk_widget_destroy() anything
	G_OBJECT_CLASS(managerEditor_parent_class)->dispose(obj);
}

static void managerEditor_finalize(GObject *obj)
{
	G_OBJECT_CLASS(managerEditor_parent_class)->finalize(obj);
}

static guint managerEditorSignals[1];

static void managerEditor_class_init(ManagerEditorClass *class)
{
	G_OBJECT_CLASS(class)->dispose = managerEditor_dispose;
	G_OBJECT_CLASS(class)->finalize = managerEditor_finalize;

	managerEditorSignals[0] = g_signal_new(
		"done", managerEditor_get_type(),
		G_SIGNAL_RUN_LAST,
		0,				// no class method slot
		NULL, NULL,		// no accumulator
		NULL,			// no marshaller
		G_TYPE_NONE,		// void clockOut(Shift *s, gpointer data);
		0);				// only specify the middle parameters; thanks larsu in irc.gimp.net/#gtk+
}

void managerEditorDone(ManagerEditor *e)
{
	g_signal_emit(e, managerEditorSignals[0], 0);
}
