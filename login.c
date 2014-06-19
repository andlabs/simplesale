// 15 june 2014
#include "simplesale.h"

struct Login {
	GtkWidget *win;
	GtkWidget *layout;
	GtkWidget *list;
	GtkWidget *listScroller;
	GtkWidget *password;
	GtkWidget *incorrect;
	GtkWidget *login;
};

static void clockedOut(Shift *s, gpointer data)
{
	Login *l = (Login *) data;

	shiftHideWindow(s);
	g_object_unref(s);
	gtk_widget_show_all(l->win);
	gtk_widget_set_sensitive(l->login, TRUE);
}

static void loginClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Login *l = (Login *) data;
//	char *password;
	Shift *s;

	gtk_widget_set_sensitive(l->login, FALSE);
//	password = gtk_entry_get_text(GTK_ENTRY(l->password));
//	gtk_entry_set_text(GTK_ENTRY(l->password), "");
//	sleep(5);
//	gtk_widget_show(l->incorrect);
//	gtk_widget_set_sensitive(l->login, TRUE);

	gtk_widget_hide(l->win);
	s = newShift("TODO");
	g_signal_connect(s, "clock-out", G_CALLBACK(clockedOut), l);
	shiftShowWindow(s);
}

static void settingsClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Login *l = (Login *) data;

	USED(l);
}

Login *newLogin(void)
{
	Login *l;
	GtkWidget *topbar;

	l = (Login *) g_malloc0(sizeof (Login));

	l->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(l->win), "simplesale");
	// TODO remove the following
	g_signal_connect(l->win, "delete-event", gtk_main_quit, NULL);

	topbar = newHeaderBar("Log In", l->win);
	newRegularHeaderButton("Settings", G_CALLBACK(settingsClicked), l, topbar);

	l->layout = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(l->layout), TRUE);

	l->list = gtk_icon_view_new();
	setAccountsModelAndIconLayout(GTK_ICON_VIEW(l->list));
	// TODO sometimes the items are too wide and this doesn't work
	gtk_icon_view_set_item_width(GTK_ICON_VIEW(l->list), -1);
	l->listScroller = newListScroller(l->list);
	gtk_grid_attach_next_to(GTK_GRID(l->layout),
		l->listScroller, NULL,
		GTK_POS_BOTTOM, 4, 1);

	l->password = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(l->password), FALSE);
	gtk_entry_set_placeholder_text(GTK_ENTRY(l->password), "Enter password here");
	gtk_widget_set_hexpand(l->password, TRUE);
	gtk_widget_set_halign(l->password, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(l->layout),
		l->password, l->listScroller,
		GTK_POS_BOTTOM, 2, 1);

	l->incorrect = gtk_label_new("Password incorrect.");
	gtk_grid_attach_next_to(GTK_GRID(l->layout),
		l->incorrect, l->password,
		GTK_POS_RIGHT, 1, 1);

	l->login = gtk_button_new_with_label("Log In");
	gtk_style_context_add_class(gtk_widget_get_style_context(l->login), "suggested-action");
	g_signal_connect(l->login, "clicked", G_CALLBACK(loginClicked), l);
	gtk_widget_set_hexpand(l->login, TRUE);
	gtk_widget_set_halign(l->login, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(l->layout),
		l->login, l->incorrect,
		GTK_POS_RIGHT, 1, 1);

	gtk_container_add(GTK_CONTAINER(l->win), l->layout);
	gtk_widget_show_all(l->win);
	gtk_widget_hide(l->incorrect);

	return l;
}
