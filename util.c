// 14 june 2014
#include "simplesale.h"

void alignLabel(GtkWidget *label, gfloat alignment)
{
	gfloat yalign;

	gtk_misc_get_alignment(GTK_MISC(label), NULL, &yalign);
	gtk_misc_set_alignment(GTK_MISC(label), alignment, yalign);
}

// in case GTK+'s default window height is too short
void expandWindowHeight(GtkWindow *win, gint factor)
{
	gint width, height;

	// the initail height is too small
	gtk_window_get_default_size(win, &width, &height);
	if (height == -1)
		gtk_window_get_size(win, NULL, &height);
	gtk_window_set_default_size(win, width, height * factor);
}

GtkWidget *newHeaderBar(char *title, GtkWidget *win)
{
	GtkWidget *topbar;

	topbar = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(topbar), title);
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(topbar), FALSE);
	gtk_window_set_titlebar(GTK_WINDOW(win), topbar);
	return topbar;
}

static GtkWidget *newHeaderButton(char *label, char *style, GCallback callback, gpointer data, GtkWidget *topbar, void (*pack)(GtkHeaderBar *, GtkWidget *))
{
	GtkWidget *button;

	button = gtk_button_new_with_label(label);
	gtk_style_context_add_class(gtk_widget_get_style_context(button), style);
	g_signal_connect(button, "clicked", callback, data);
	(*pack)(GTK_HEADER_BAR(topbar), button);
	return button;
}

GtkWidget *newConfirmHeaderButton(char *label, GCallback callback, gpointer data, GtkWidget *topbar)
{
	return newHeaderButton(label, "suggested-action", callback, data, topbar, gtk_header_bar_pack_start);
}

GtkWidget *newCancelHeaderButton(char *label, GCallback callback, gpointer data, GtkWidget *topbar)
{
	return newHeaderButton(label, "destructive-action", callback, data, topbar, gtk_header_bar_pack_end);
}

GtkWidget *newListScroller(GtkWidget *list)
{
	GtkWidget *listScroller;

	listScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(listScroller), list);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(listScroller), GTK_SHADOW_IN);
	gtk_widget_set_hexpand(listScroller, TRUE);
	gtk_widget_set_halign(listScroller, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(listScroller, TRUE);
	gtk_widget_set_valign(listScroller, GTK_ALIGN_FILL);
	return listScroller;
}

void attachLabel(char *text, GtkWidget *widget, GtkWidget *grid)
{
	GtkWidget *label;

	label = gtk_label_new(text);
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(grid),
		label, widget,
		GTK_POS_LEFT, 1, 1);
}

gboolean askConfirm(GtkWidget *parent, char *secondary, char *pformat, ...)
{
	gint response;
	GtkWidget *alert;
	char *primary;
	va_list arg;

	va_start(arg, pformat);
	primary = g_strdup_vprintf(pformat, arg);
	va_end(arg);
	alert = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL,
		GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
		"%s", primary);
	if (secondary != NULL && *secondary != '\0')		// only if there is secondary text
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(alert),
			"%s", secondary);
	response = gtk_dialog_run(GTK_DIALOG(alert));
	gtk_widget_destroy(alert);
	g_free(primary);
	return response == GTK_RESPONSE_YES;
}
