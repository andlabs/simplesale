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

	gboolean selected;
	GtkTreeIter selection;
};

static void clockedOut(Shift *s, gpointer data)
{
	Login *l = (Login *) data;

	shiftHideWindow(s);
	g_object_unref(s);
	gtk_widget_show_all(l->win);
	gtk_widget_set_sensitive(l->login, TRUE);
}

// all because GtkIconView doesn't use GtkTreeSelection...
static void accountSelected(GtkIconView *iconview, gpointer data)
{
	USED(iconview);

	Login *l = (Login *) data;
	GList *selected;
	guint n;

	selected = gtk_icon_view_get_selected_items(GTK_ICON_VIEW(l->list));
	if (selected == NULL)
		goto nothing;
	n = g_list_length(selected);
	if (n == 0)
		goto nothing;
	if (n > 1)
		g_error("multiple accounts selected (icon view should have been set to single-selection)");
	l->selected = TRUE;
	gtk_tree_model_get_iter(gtk_icon_view_get_model(GTK_ICON_VIEW(l->list)),
		&l->selection, (GtkTreePath *) selected->data);
	goto done;

nothing:
	l->selected = FALSE;
	// fall through

done:
	gtk_widget_set_sensitive(l->password, l->selected);
	gtk_widget_set_sensitive(l->login, l->selected);
	gtk_entry_set_text(GTK_ENTRY(l->password), "");
	gtk_widget_hide(l->incorrect);
	if (selected != NULL)
		g_list_free_full(selected, (GDestroyNotify) gtk_tree_path_free);
}

static void loginClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Login *l = (Login *) data;
	const char *password;
	Shift *s;

	if (!l->selected)
		g_error("Login clicked with no item selected (should be disabled");
	password = gtk_entry_get_text(GTK_ENTRY(l->password));
	if (!matches(password, &l->selection)) {
		// TODO delay?
		gtk_icon_view_unselect_all(GTK_ICON_VIEW(l->list));
		gtk_widget_show(l->incorrect);
		return;
	}
	gtk_widget_hide(l->win);
	gtk_icon_view_unselect_all(GTK_ICON_VIEW(l->list));
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
	GtkWidget *incorrectLabel;
	GtkBox *box;
	gint response;

	pwprompt = gtk_message_dialog_new(GTK_WINDOW(l->win), GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
		"Enter the Manager's password and click OK to access the Manager Control Panel.");
	gtk_message_dialog_set_image(GTK_MESSAGE_DIALOG(pwprompt),
		gtk_image_new_from_icon_name("dialog-password", GTK_ICON_SIZE_DIALOG));			// size used by GtkMessageDialog already, so eh
	box = GTK_BOX(gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(pwprompt)));
	pwentry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(pwentry), FALSE);
	gtk_box_pack_start(box, pwentry, TRUE, TRUE, 0);
	incorrectLabel = gtk_label_new("Password incorrect.");
	alignLabel(incorrectLabel, 0);
	// TODO format?
	gtk_box_pack_start(box, incorrectLabel, TRUE, TRUE, 0);
	gtk_widget_show_all(pwprompt);		// both the icon and the entry are hidden by default
	gtk_widget_hide(incorrectLabel);		// but hide the label
	for (;;) {
		response = gtk_dialog_run(GTK_DIALOG(pwprompt));
		if (response != GTK_RESPONSE_OK) {
			printf("cancelled\n");
			gtk_widget_destroy(pwprompt);
			return;
		}
		if (matchesManagerPassword(gtk_entry_get_text(GTK_ENTRY(pwentry))))
			break;
		gtk_widget_show(incorrectLabel);
	}
	gtk_widget_destroy(pwprompt);
	printf("pass\n");
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
	g_signal_connect(l->list, "selection-changed", G_CALLBACK(accountSelected), l);
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

	accountSelected(NULL, l);		// initial state

	return l;
}
