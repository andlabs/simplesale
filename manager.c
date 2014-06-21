// 21 june 2014
#include "simplesale.h"

struct Manager {
	GtkWidget *win;
};

// TODO make ItemEditor, et al. subclass something for this
static void returnToManager(GObject *obj, gpointer data)
{
	Manager *m = (Manager *) data;

	g_object_unref(obj);
	gtk_widget_show_all(m->win);
}

static void itemEditorClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Manager *m = (Manager *) data;
	ItemEditor *e;

	gtk_widget_hide(m->win);
	e = newItemEditor();
	g_signal_connect(e, "done", G_CALLBACK(returnToManager), m);
	itemEditorShow(e);
}

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
	BUTTON("contact-new", "Account Editor", gtk_main_quit, 0, 1);
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
