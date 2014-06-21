// 21 june 2014
#include "simplesale.h"

struct AccountEditor {
	GtkWidget *win;
	GtkWidget *layout;
	GtkWidget *leftside;
	GtkWidget *add;
	GtkWidget *list;
	GtkTreeSelection *listSel;		// convenience
	GtkWidget *listScroller;
	GtkWidget *rightside;
	GtkWidget *name;
	GtkWidget *passentry;
	GtkWidget *change;

	GtkTreeIter current;
	gboolean selecting;
};

static void saveClicked(GtkButton *button, gpointer data)
{
	USED(button);

	AccountEditor *e = (AccountEditor *) data;

	saveAccounts();
	gtk_main_quit();USED(e);// TODO signal completion
}

static void discardClicked(GtkButton *button, gpointer data)
{
	USED(button);

	AccountEditor *e = (AccountEditor *) data;

	if (!askConfirm(e->win, NULL,
		"Are you sure you want to discard all changes and leave the Account Editor? This will also discard password changes!"))
		return;
	initAccounts();
	gtk_main_quit();USED(e);// TODO signal completion
}

static void addClicked(GtkButton *button, gpointer data)
{
	USED(button);

	AccountEditor *e = (AccountEditor *) data;
	char *newpass;
	GtkTreeIter iter;

	newpass = askNewPassword(e->win,
		"Enter a password for this new account. You can set the account's name and other properties afterward.",
		"create the new account");
	// TODO ask the person himself to enter the password
	if (newpass == NULL)
		return;
	iter = addAccount("New Account", newpass);
	gtk_tree_selection_select_iter(e->listSel, &iter);
}

static void accountSelected(GtkTreeSelection *selection, gpointer data)
{
	USED(selection);

	AccountEditor *e = (AccountEditor *) data;
	gboolean selected;
	char *name = "";

	selected = gtk_tree_selection_get_selected(e->listSel, NULL, &e->current);
	gtk_widget_set_sensitive(e->name, selected);
	gtk_widget_set_sensitive(e->passentry, selected);
	if (selected)
		name = accountName(&e->current);
	e->selecting = TRUE;
	gtk_entry_set_text(GTK_ENTRY(e->name), name);
	e->selecting = FALSE;
	resetPassEntry(PASS_ENTRY(e->passentry));
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
	setAccountName(&iter, gtk_entry_get_text(GTK_ENTRY(e->name)));
}

static void changeClicked(GtkButton *button, gpointer data)
{
	USED(button);

	AccountEditor *e = (AccountEditor *) data;
	GtkTreeIter iter;
	const char *curpass;

	if (gtk_tree_selection_get_selected(e->listSel, NULL, &iter) == FALSE)
		g_error("change password clicked without any account selected (button should be disabled)");
	curpass = passEntryCurrentPassword(PASS_ENTRY(e->passentry));
	if (!matches(curpass, &iter)) {
		printf("password mismatch\n");
		return;
	}
	setAccountPassword(&iter, passEntryNewPassword(PASS_ENTRY(e->passentry)));
	printf("password changed\n");
}

AccountEditor *newAccountEditor(void)
{
	AccountEditor *e;
	GtkWidget *topbar;
	GtkWidget *groupbox;
	GtkWidget *groupgrid;

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

	e->leftside = gtk_grid_new();

	e->add = gtk_button_new_with_label("Add Account");
	g_signal_connect(e->add, "clicked", G_CALLBACK(addClicked), e);
	gtk_widget_set_hexpand(e->add, TRUE);
	gtk_widget_set_halign(e->add, GTK_ALIGN_END);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		e->add, NULL,
		GTK_POS_TOP, 1, 1);

	e->list = gtk_tree_view_new();
	setAccountsModelAndColumnLayout(GTK_TREE_VIEW(e->list));
	e->listSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(e->list));
	// TODO figure out how to make it so that clicking on blank space deselects
	g_signal_connect(e->listSel, "changed", G_CALLBACK(accountSelected), e);
	e->listScroller = newListScroller(e->list);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		e->listScroller, e->add,
		GTK_POS_BOTTOM, 1, 1);

	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->leftside, NULL,
		GTK_POS_TOP, 1, 1);

	e->rightside = gtk_grid_new();

	groupbox = gtk_frame_new("General (changes applied immediately)");
	groupgrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(groupbox), groupgrid);

	e->name = gtk_entry_new();
	g_signal_connect(e->name, "changed", G_CALLBACK(nameChanged), e);
	gtk_widget_set_hexpand(e->name, TRUE);
	gtk_widget_set_halign(e->name, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(groupgrid),
		e->name, NULL,
		GTK_POS_RIGHT, 2, 1);
	attachLabel("Name:", e->name, groupgrid);

	gtk_widget_set_hexpand(groupbox, TRUE);
	gtk_widget_set_halign(groupbox, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		groupbox, NULL,
		GTK_POS_TOP, 1, 1);

	groupbox = gtk_frame_new("Password (changes when Change clicked)");
	groupgrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(groupbox), groupgrid);

	e->change = gtk_button_new_with_label("Change");
	g_signal_connect(e->change, "clicked", G_CALLBACK(changeClicked), e);
	gtk_widget_set_hexpand(e->change, FALSE);
	gtk_widget_set_halign(e->change, GTK_ALIGN_START);
	// don't attach; see what we do next

	e->passentry = newPassEntry(TRUE, e->change);
	g_object_bind_property(e->name, "text",
		e->passentry, "account-name",
		G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);
	gtk_widget_set_hexpand(e->passentry, TRUE);
	gtk_widget_set_halign(e->passentry, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(groupgrid),
		e->passentry, NULL,
		GTK_POS_TOP, 1, 1);

	gtk_widget_set_hexpand(groupbox, TRUE);
	gtk_widget_set_halign(groupbox, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		groupbox, NULL,
		GTK_POS_BOTTOM, 1, 1);

	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->rightside, e->leftside,
		GTK_POS_RIGHT, 2, 1);

	gtk_container_add(GTK_CONTAINER(e->win), e->layout);
	gtk_widget_show_all(e->win);

	accountSelected(NULL, e);		// set up initial state

	return e;
}
