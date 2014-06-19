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

static void managerClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Login *l = (Login *) data;
	GtkWidget *pwprompt;
	GtkWidget *pwentry;
	GtkBox *box;
	gint response;
	char *pw;

	pwprompt = gtk_message_dialog_new(GTK_WINDOW(l->win), GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
		"Enter the Manager's password and click OK to access the [TODO]");
	gtk_message_dialog_set_image(GTK_MESSAGE_DIALOG(pwprompt),
		gtk_image_new_from_icon_name("dialog-password", GTK_ICON_SIZE_DIALOG));			// size used by GtkMessageDialog already, so eh
	box = GTK_BOX(gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(pwprompt)));
	pwentry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(pwentry), FALSE);
	gtk_box_pack_end(box, pwentry, TRUE, TRUE, 0);
	gtk_widget_show_all(pwprompt);		// both the icon and the entry are hidden by default
	response = gtk_dialog_run(GTK_DIALOG(pwprompt));
	pw = g_strdup(gtk_entry_get_text(GTK_ENTRY(pwentry)));
	gtk_widget_destroy(pwprompt);

	USED(response);

	g_free(pw);
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
	newRegularHeaderButton("Manager", G_CALLBACK(managerClicked), l, topbar);

	l->layout = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(l->layout), TRUE);

	l->list = gtk_icon_view_new();
	setAccountsModelAndIconLayout(GTK_ICON_VIEW(l->list));
	// TODO the default icon size is somehow making each item on the list have a lot of padding
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
