// 15 june 2014
#include "simplesale.h"

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

static void loginClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Login *l = (Login *) data;

	gtk_widget_set_sensitive(l->login, FALSE);
	sleep(5);
	gtk_widget_show(l->incorrect);
	gtk_widget_set_sensitive(l->login, TRUE);
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
