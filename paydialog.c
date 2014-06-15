// 15 june 2014
#include "simplesale.h"

struct payDialog {
	GtkWidget *dialog;
	GtkWidget *layout;
	GtkWidget *dollars;
	GtkWidget *cents;
	GtkWidget *buttongrid;
	GtkWidget *digits[10];
	GtkWidget *decimal;
};

static char *digitstrings[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

payDialog *newPayDialog(GtkWindow *parent, order *o)
{
	payDialog *p;
	GtkWidget *label, *firstlabel;
	gchar *pricestr;
	int i;

	p = (payDialog *) g_malloc0(sizeof (payDialog));

	p->dialog = gtk_dialog_new_with_buttons("Pay Now",
		parent, GTK_DIALOG_MODAL,
		"_Pay", GTK_RESPONSE_ACCEPT,
		"_Cancel", GTK_RESPONSE_REJECT,
		NULL);

	p->layout = gtk_grid_new();

	label = gtk_label_new("Order Total:");
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, NULL,
		GTK_POS_TOP, 1, 1);
	firstlabel = label;
	// TODO total not subtotal
	pricestr = priceToString(subtotal(o), "$");
	label = gtk_label_new(pricestr);
	g_free(pricestr);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, firstlabel,
		GTK_POS_RIGHT, 4, 1);

	label = gtk_label_new("Amount Given:");
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, firstlabel,
		GTK_POS_BOTTOM, 1, 1);
	firstlabel = label;
	label = gtk_label_new("$");
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, firstlabel,
		GTK_POS_RIGHT, 1, 1);
	firstlabel = label;		// for button grid
	p->dollars = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(p->dollars), 4);
	// TODO restrict input to numeric
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		p->dollars, label,
		GTK_POS_RIGHT, 1, 1);
	label = gtk_label_new(".");
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, p->dollars,
		GTK_POS_RIGHT, 1, 1);
	p->cents = gtk_spin_button_new_with_range(0, 99, 1);
	gtk_spin_button_set_digits(GTK_SPIN_BUTTON(p->cents), 0);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(p->cents), TRUE);
	// TODO leading zero
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		p->cents, label,
		GTK_POS_RIGHT, 1, 1);

	p->buttongrid = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(p->buttongrid), TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(p->buttongrid), TRUE);
	for (i = 0; i < 10; i++) {
		p->digits[i] = gtk_button_new_with_label(digitstrings[i]);
		gtk_style_context_add_class(gtk_widget_get_style_context(p->digits[i]), "image-button");
	}
	gtk_grid_attach_next_to(GTK_GRID(p->buttongrid),
		p->digits[7], NULL,
		GTK_POS_TOP, 1, 1);
#define ADDBUTTON(i, j, pos) gtk_grid_attach_next_to(GTK_GRID(p->buttongrid), p->digits[(i)], p->digits[(j)], (pos), 1, 1)
	ADDBUTTON(8, 7, GTK_POS_RIGHT);
	ADDBUTTON(9, 8, GTK_POS_RIGHT);
	ADDBUTTON(4, 7, GTK_POS_BOTTOM);
	ADDBUTTON(5, 4, GTK_POS_RIGHT);
	ADDBUTTON(6, 5, GTK_POS_RIGHT);
	ADDBUTTON(1, 4, GTK_POS_BOTTOM);
	ADDBUTTON(2, 1, GTK_POS_RIGHT);
	ADDBUTTON(3, 2, GTK_POS_RIGHT);
	ADDBUTTON(0, 2, GTK_POS_BOTTOM);
	p->decimal = gtk_button_new_with_label(".");
	gtk_style_context_add_class(gtk_widget_get_style_context(p->decimal), "image-button");
	gtk_grid_attach_next_to(GTK_GRID(p->buttongrid),
		p->decimal, p->digits[0],
		GTK_POS_RIGHT, 1, 1);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		p->buttongrid, firstlabel,
		GTK_POS_BOTTOM, 4, 1);

	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(p->dialog))), p->layout);

	return p;
}

gint runAndFreePayDialog(payDialog *p)
{
	gint result;

	gtk_widget_show_all(p->dialog);
	result = gtk_dialog_run(GTK_DIALOG(p->dialog));
	gtk_widget_destroy(p->dialog);		// TODO does this destroy subwidgets?
	g_free(p);
	return result;
}
