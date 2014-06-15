// 15 june 2014
#include "simplesale.h"

struct shift {
	char *employee;
	GHashTable *orders;

	GtkWidget *win;
	GtkWidget *layout;
	GtkWidget *list;
	GtkWidget *listScroller;
	GtkWidget *resume;
};

shift *newShift(char *name)
{
	shift *s;
	gint width, height;
	GtkWidget *topbar;
	GtkWidget *button;
	GtkWidget *label;

	s = (shift *) g_malloc0(sizeof (shift));
	s->employee = g_strdup(name);
	s->orders = g_hash_table_new(g_direct_hash, g_direct_equal);

	s->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(s->win), "simplesale");
	// TODO remove the following
	g_signal_connect(s->win, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	gtk_window_get_default_size(GTK_WINDOW(s->win), &width, &height);
	if (height == -1)
		gtk_window_get_size(GTK_WINDOW(s->win), NULL, &height);
	gtk_window_set_default_size(GTK_WINDOW(s->win), width, height * 2);

	topbar = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(topbar), s->employee);
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(topbar), FALSE);
	gtk_window_set_titlebar(GTK_WINDOW(s->win), topbar);

	button = gtk_button_new_with_label("New Order");
	gtk_style_context_add_class(gtk_widget_get_style_context(button), "suggested-action");
//	g_signal_connect(button, "clicked", G_CALLBACK(newOrder), s);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(topbar), button);

	button = gtk_button_new_with_label("Clock Out");
	gtk_style_context_add_class(gtk_widget_get_style_context(button), "destructive-action");
//	g_signal_connect(button, "clicked", G_CALLBACK(clockOut), s);
	g_signal_connect(button, "clicked", gtk_main_quit, NULL);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(topbar), button);

	s->layout = gtk_grid_new();

	label = gtk_label_new("Orders for later:");
	alignLabel(label, 0);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(s->layout),
		label, NULL,
		GTK_POS_TOP, 1, 1);

	s->list = gtk_list_box_new();
	s->listScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(s->listScroller), s->list);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(s->listScroller), GTK_SHADOW_IN);
	gtk_widget_set_hexpand(s->listScroller, TRUE);
	gtk_widget_set_halign(s->listScroller, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(s->listScroller, TRUE);
	gtk_widget_set_valign(s->listScroller, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(s->layout),
		s->listScroller, label,
		GTK_POS_BOTTOM, 1, 1);

	s->resume = gtk_button_new_with_label("Resume Order");
	gtk_widget_set_sensitive(s->resume, FALSE);		// initial state; no items present
//	TODO resume
	gtk_widget_set_hexpand(s->resume, TRUE);
	gtk_widget_set_halign(s->resume, GTK_ALIGN_END);
	gtk_grid_attach_next_to(GTK_GRID(s->layout),
		s->resume, s->listScroller,
		GTK_POS_BOTTOM, 1, 1);

	gtk_container_add(GTK_CONTAINER(s->win), s->layout);
	gtk_widget_show_all(s->win);

	return s;
}

void shiftNewOrder(shift *s)
{
	order *o;
	orderWindow *ow;

	o = newOrder();
	// sample items
	{
		addItem("Regular Slice", PRICE(2, 00));
		addItem("Large Soda", PRICE(1, 50));
		addItem("Cookie", PRICE(1, 00));
		addToOrder(o, 0);
		addToOrder(o, 1);
	}
	ow = newOrderWindow(s, o);
	g_hash_table_insert(s->orders, o, ow);
}

void shiftDoOrder(shift *s, order *o, int action)
{
	USED(s);
	// TODO get out orderWindow
	USED(o);
	switch (action) {
	case orderCancel:
		printf("order cancelled\n");
		break;
	case orderPayNow:
		printf("order paid now\n");
		break;
	case orderPayLater:
		printf("order paid later\n");
		break;
	}
	gtk_main_quit();
}
