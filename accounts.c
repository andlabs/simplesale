// 18 june 2014
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

void setAccountsModelAndIconLayout(GtkIconView *list)
{
	gtk_icon_view_set_model(list, GTK_TREE_MODEL(accounts));
	gtk_icon_view_set_text_column(list, 0);
	gtk_icon_view_set_pixbuf_column(list, 2);
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

	if (!askConfirm(e->win, NULL,
		"Are you sure you want to discard all changes and leave the Account Editor? This will also discard password changes!"))
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
	AccountEditor *e;
	GtkWidget *topbar;
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;

	e = (AccountEditor *) g_malloc0(sizeof (AccountEditor));

	e->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(e->win), "simplesale");
	// TODO remove the following
	g_signal_connect(e->win, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	expandWindowHeight(GTK_WINDOW(e->win), 2);

	topbar = newHeaderBar("Account Editor", e->win);
	newConfirmHeaderButton("Save and Close", G_CALLBACK(saveClicked), e, topbar);
	newCancelHeaderButton("Discard and Close", G_CALLBACK(discardClicked), e, topbar);

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
	e->listScroller = newListScroller(e->list);
	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->listScroller, NULL,
		GTK_POS_TOP, 1, 1);

	e->rightside = gtk_grid_new();

	e->name = gtk_entry_new();
	g_signal_connect(e->name, "changed", G_CALLBACK(nameChanged), e);
	gtk_widget_set_hexpand(e->name, TRUE);
	gtk_widget_set_halign(e->name, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->name, NULL,
		GTK_POS_RIGHT, 2, 1);
	attachLabel("Name:", e->name, e->rightside);

	e->curpass = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(e->curpass), FALSE);
	g_signal_connect(e->curpass, "changed", G_CALLBACK(passwordChanged), e);
	gtk_widget_set_vexpand(e->curpass, FALSE);
	gtk_widget_set_valign(e->curpass, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->curpass, e->name,
		GTK_POS_BOTTOM, 1, 1);
	attachLabel("Current Password:\n(if changing)", e->curpass, e->rightside);

	e->newpass = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(e->newpass), FALSE);
	g_signal_connect(e->newpass, "changed", G_CALLBACK(passwordChanged), e);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->newpass, e->curpass,
		GTK_POS_BOTTOM, 1, 1);
	attachLabel("New Password:", e->newpass, e->rightside);

	e->passlevel = gtk_level_bar_new_for_interval(0, 100);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->passlevel, e->newpass,
		GTK_POS_BOTTOM, 1, 1);
	e->passleveltext = gtk_label_new("");
	alignLabel(e->passleveltext, 0);
	// TODO word wrap
	gtk_widget_set_hexpand(e->passleveltext, TRUE);
	gtk_widget_set_halign(e->passleveltext, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->passleveltext, e->passlevel,
		GTK_POS_RIGHT, 1, 1);
	attachLabel("Password Strength:", e->passlevel, e->rightside);

	e->confirmpass = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(e->confirmpass), FALSE);
	g_signal_connect(e->confirmpass, "changed", G_CALLBACK(passwordChanged), e);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->confirmpass, e->passlevel,
		GTK_POS_BOTTOM, 1, 1);
	e->mismatch = gtk_label_new("");
	alignLabel(e->mismatch, 0);
	gtk_widget_set_hexpand(e->mismatch, TRUE);
	gtk_widget_set_halign(e->mismatch, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->mismatch, e->confirmpass,
		GTK_POS_RIGHT, 1, 1);
	attachLabel("Retype New Password:", e->confirmpass, e->rightside);

	// TODO add an undo button?

	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->rightside, e->listScroller,
		GTK_POS_RIGHT, 2, 1);

	gtk_container_add(GTK_CONTAINER(e->win), e->layout);
	gtk_widget_show_all(e->win);

	accountSelected(NULL, e);		// set up initial state

	return e;
}
