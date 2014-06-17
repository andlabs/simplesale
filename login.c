// 15 june 2014
#include "simplesale.h"
#include <pwquality.h>

static GtkListStore *accounts;
static GdkPixbuf *accountIcon;

void initAccounts(void)
{
	accounts = gtk_list_store_new(3,
		G_TYPE_STRING,		// name
		G_TYPE_STRING,		// password
		GDK_TYPE_PIXBUF);		// icon - TODO correct type?
	accountIcon = gtk_icon_theme_load_icon(
		gtk_icon_theme_get_default(),
		"face-smile",	// just a placeholder face for now; thanks Lumpio- in irc.freenode.net/#freedesktop
		48,
		0,			// no custom flags
		NULL);		// TODO handle error
}

void addAccount(char *name, char *password)
{
	GtkTreeIter iter;

	gtk_list_store_append(accounts, &iter);
	gtk_list_store_set(accounts, &iter,
		0, name,
		1, password,		// TODO bcrypt
		2, accountIcon,
		-1);
}

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

Login *newLogin(void)
{
	Login *l;
	GtkWidget *topbar;

	l = (Login *) g_malloc0(sizeof (Login));

	l->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(l->win), "simplesale");
	// TODO remove the following
	g_signal_connect(l->win, "delete-event", gtk_main_quit, NULL);

	topbar = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(topbar), "Log In");
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(topbar), FALSE);
	gtk_window_set_titlebar(GTK_WINDOW(l->win), topbar);

	l->layout = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(l->layout), TRUE);

	l->list = gtk_icon_view_new_with_model(GTK_TREE_MODEL(accounts));
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(l->list), 0);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(l->list), 2);
	gtk_icon_view_set_item_width(GTK_ICON_VIEW(l->list), -1);
	l->listScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(l->listScroller), l->list);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(l->listScroller), GTK_SHADOW_IN);
	gtk_widget_set_hexpand(l->listScroller, TRUE);
	gtk_widget_set_halign(l->listScroller, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(l->listScroller, TRUE);
	gtk_widget_set_valign(l->listScroller, GTK_ALIGN_FILL);
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

// this is the GUI for editing accounts

struct AccountEditor {
	GtkWidget *win;
	GtkWidget *layout;
	GtkWidget *list;
	GtkTreeSelection *listSel;		// convenience
	GtkWidget *listScroller;
	GtkWidget *rightside;
	GtkWidget *name;
	GtkWidget *curpass;
	GtkWidget *newpass;
	GtkWidget *passlevel;
	GtkWidget *passleveltext;
	GtkWidget *confirmpass;
	GtkWidget *mismatch;

	GtkTreeIter current;
	gboolean selecting;
};

static void saveClicked(GtkButton *button, gpointer data)
{
	USED(button);

	AccountEditor *e = (AccountEditor *) data;

//	saveAccounts();		// TODO
	gtk_main_quit();USED(e);// TODO signal completion
}

static void discardClicked(GtkButton *button, gpointer data)
{
	USED(button);

	AccountEditor *e = (AccountEditor *) data;
	GtkWidget *alert;
	gint response;

	alert = gtk_message_dialog_new(GTK_WINDOW(e->win), GTK_DIALOG_MODAL,
		GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
		"Are you sure you want to discard all changes and leave the Account Editor? This will also discard password changes!");
	response = gtk_dialog_run(GTK_DIALOG(alert));
	gtk_widget_destroy(alert);
	if (response != GTK_RESPONSE_YES)
		return;
	g_object_unref(accounts);
//	initAccounts();		// TODO
	gtk_main_quit();USED(e);// TODO signal completion
}

static void accountSelected(GtkTreeSelection *selection, gpointer data)
{
	USED(selection);

	AccountEditor *e = (AccountEditor *) data;
	gboolean selected;
	char *name = "";

	selected = gtk_tree_selection_get_selected(e->listSel, NULL, &e->current);
	gtk_widget_set_sensitive(e->name, selected);
	gtk_widget_set_sensitive(e->curpass, selected);
	gtk_widget_set_sensitive(e->newpass, selected);
	gtk_widget_set_sensitive(e->confirmpass, selected);
	if (selected)
		gtk_tree_model_get(GTK_TREE_MODEL(accounts), &e->current, 0, &name, -1);
	else {
		gtk_level_bar_set_value(GTK_LEVEL_BAR(e->passlevel), 0);
		gtk_label_set_text(GTK_LABEL(e->passleveltext), "");
		gtk_label_set_text(GTK_LABEL(e->mismatch), "");
	}
	e->selecting = TRUE;
	gtk_entry_set_text(GTK_ENTRY(e->name), name);
	e->selecting = FALSE;
	gtk_entry_set_text(GTK_ENTRY(e->curpass), "");
	gtk_entry_set_text(GTK_ENTRY(e->newpass), "");
	gtk_entry_set_text(GTK_ENTRY(e->confirmpass), "");
	gtk_level_bar_set_value(GTK_LEVEL_BAR(e->passlevel), 0);
	gtk_label_set_text(GTK_LABEL(e->passleveltext), "");
}

static void nameChanged(GtkEditable *editable, gpointer data)
{
	USED(editable);

	AccountEditor *e = (AccountEditor *) data;
	GtkTreeIter iter;

	if (e->selecting)	// prevent spurious g_error() during selection changed
		return;
	if (gtk_tree_selection_get_selected(e->listSel, NULL, &iter) == FALSE)
		g_error("account name changed without any account selected (textbox should be disabled)");
	gtk_list_store_set(accounts, &iter, 0, gtk_entry_get_text(GTK_ENTRY(e->name)), -1);
}

static void passwordChanged(GtkEditable *editable, gpointer data)
{
	USED(editable);

	AccountEditor *e = (AccountEditor *) data;
	const char *name, *cur, *new, *confirm;
	int level;

	name = gtk_entry_get_text(GTK_ENTRY(e->name));
	cur = gtk_entry_get_text(GTK_ENTRY(e->curpass));
	new = gtk_entry_get_text(GTK_ENTRY(e->newpass));
	confirm = gtk_entry_get_text(GTK_ENTRY(e->newpass));
	level = pwquality_check(pwquality_default_settings(), new, cur, name, NULL);
	if (level >= 0) {
		gtk_level_bar_set_value(GTK_LEVEL_BAR(e->passlevel), (gdouble) level);
		gtk_label_set_text(GTK_LABEL(e->passleveltext), "");
	} else {
		gtk_level_bar_set_value(GTK_LEVEL_BAR(e->passlevel), 0);
		gtk_label_set_text(GTK_LABEL(e->passleveltext), pwquality_strerror(NULL, 0, level, NULL));
	}
	if (strcmp(new, confirm) != 0)
		gtk_label_set_text(GTK_LABEL(e->mismatch), "Passwords do not match.");
	else
		gtk_label_set_text(GTK_LABEL(e->mismatch), "");
}

// TODO change password clicked

AccountEditor *newAccountEditor(void)
{
	gint width, height;
	AccountEditor *e;
	GtkWidget *topbar;
	GtkWidget *button;
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;
	GtkWidget *label;

	e = (AccountEditor *) g_malloc0(sizeof (AccountEditor));

	e->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(e->win), "simplesale");
	// TODO remove the following
	g_signal_connect(e->win, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	gtk_window_get_default_size(GTK_WINDOW(e->win), &width, &height);
	if (height == -1)
		gtk_window_get_size(GTK_WINDOW(e->win), NULL, &height);
	gtk_window_set_default_size(GTK_WINDOW(e->win), width, height * 2);

	topbar = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(topbar), "Account Editor");
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(topbar), FALSE);
	gtk_window_set_titlebar(GTK_WINDOW(e->win), topbar);

	button = gtk_button_new_with_label("Save and Close");
	gtk_style_context_add_class(gtk_widget_get_style_context(button), "suggested-action");
	g_signal_connect(button, "clicked", G_CALLBACK(saveClicked), e);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(topbar), button);

	button = gtk_button_new_with_label("Discard and Close");
	gtk_style_context_add_class(gtk_widget_get_style_context(button), "destructive-action");
	g_signal_connect(button, "clicked", G_CALLBACK(discardClicked), e);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(topbar), button);

	e->layout = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(e->layout), TRUE);

	e->list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(accounts));
	e->listSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(e->list));
	// TODO figure out how to make it so that clicking on blank space deselects
	g_signal_connect(e->listSel, "changed", G_CALLBACK(accountSelected), e);
	r = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("", r, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(e->list), col);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(e->list), FALSE);
	e->listScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(e->listScroller), e->list);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(e->listScroller), GTK_SHADOW_IN);
	gtk_widget_set_hexpand(e->listScroller, TRUE);
	gtk_widget_set_halign(e->listScroller, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(e->listScroller, TRUE);
	gtk_widget_set_valign(e->listScroller, GTK_ALIGN_FILL);

	e->rightside = gtk_grid_new();
	label = gtk_label_new("Name:");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		label, NULL,
		GTK_POS_TOP, 1, 1);
	e->name = gtk_entry_new();
	g_signal_connect(e->name, "changed", G_CALLBACK(nameChanged), e);
	gtk_widget_set_hexpand(e->name, TRUE);
	gtk_widget_set_halign(e->name, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->name, label,
		GTK_POS_RIGHT, 2, 1);
{GtkWidget *l2=label;
	label = gtk_label_new("Current Password:\n(if changing)");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		label, l2,
		GTK_POS_BOTTOM, 1, 1);
}
	e->curpass = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(e->curpass), FALSE);
	g_signal_connect(e->curpass, "changed", G_CALLBACK(passwordChanged), e);
	gtk_widget_set_vexpand(e->curpass, FALSE);
	gtk_widget_set_valign(e->curpass, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->curpass, label,
		GTK_POS_RIGHT, 1, 1);
{GtkWidget *l2=label;
	label = gtk_label_new("New Password:");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		label, l2,
		GTK_POS_BOTTOM, 1, 1);
}
	e->newpass = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(e->newpass), FALSE);
	g_signal_connect(e->newpass, "changed", G_CALLBACK(passwordChanged), e);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->newpass, label,
		GTK_POS_RIGHT, 1, 1);
{GtkWidget *l2=label;
	label = gtk_label_new("Password Strength:");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		label, l2,
		GTK_POS_BOTTOM, 1, 1);
}
	e->passlevel = gtk_level_bar_new_for_interval(0, 100);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->passlevel, label,
		GTK_POS_RIGHT, 1, 1);
	e->passleveltext = gtk_label_new("");
	alignLabel(e->passleveltext, 0);
	gtk_widget_set_hexpand(e->passleveltext, TRUE);
	gtk_widget_set_halign(e->passleveltext, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->passleveltext, e->passlevel,
		GTK_POS_RIGHT, 1, 1);
{GtkWidget *l2=label;
	label = gtk_label_new("Retype New Password:");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		label, l2,
		GTK_POS_BOTTOM, 1, 1);
}
	e->confirmpass = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(e->confirmpass), FALSE);
	g_signal_connect(e->confirmpass, "changed", G_CALLBACK(passwordChanged), e);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->confirmpass, label,
		GTK_POS_RIGHT, 1, 1);
	e->mismatch = gtk_label_new("");
	alignLabel(e->mismatch, 0);
	gtk_widget_set_hexpand(e->mismatch, TRUE);
	gtk_widget_set_halign(e->mismatch, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->mismatch, e->confirmpass,
		GTK_POS_RIGHT, 1, 1);

	// TODO add an undo button?

	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->listScroller, NULL,
		GTK_POS_TOP, 1, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->rightside, e->listScroller,
		GTK_POS_RIGHT, 2, 1);

	gtk_container_add(GTK_CONTAINER(e->win), e->layout);
	gtk_widget_show_all(e->win);

	accountSelected(NULL, e);		// set up initial state

	return e;
}
