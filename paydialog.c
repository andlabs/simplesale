// 15 june 2014
#include "simplesale.h"

struct PayDialog {
	Price price;

	GtkWidget *dialog;
	GtkWidget *layout;
	GtkWidget *cost;
	GtkWidget *amount;
	GtkWidget *buttongrid;
	GtkWidget *digits[10];
	GtkWidget *decimal;
};

static char *digitstrings[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

PayDialog *newPayDialog(GtkWindow *parent, Price price)
{
	PayDialog *p;
	int i;

	p = (PayDialog *) g_malloc0(sizeof (PayDialog));
	p->price = price;

	p->dialog = gtk_dialog_new_with_buttons("Pay Now",
		parent, GTK_DIALOG_MODAL,
		"_Pay", GTK_RESPONSE_ACCEPT,
		"_Cancel", GTK_RESPONSE_REJECT,
		NULL);
	// TODO set primary/secondary buttons

	p->layout = gtk_grid_new();

	// TODO make this look like a label
	p->cost = newPriceEntry();
	priceEntrySetPrice(PRICE_ENTRY(p->cost), p->price);
	gtk_widget_set_sensitive(p->cost, FALSE);
	gtk_widget_set_hexpand(p->cost, TRUE);
	gtk_widget_set_halign(p->cost, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		p->cost, NULL,
		GTK_POS_RIGHT, 1, 1);
	attachLabel("Order Total:", p->cost, p->layout);

	p->amount = newPriceEntry();
	gtk_widget_set_hexpand(p->amount, TRUE);
	gtk_widget_set_halign(p->amount, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		p->amount, p->cost,
		GTK_POS_BOTTOM, 1, 1);
	attachLabel("Amount Given:", p->amount, p->layout);

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
	gtk_widget_set_hexpand(p->buttongrid, TRUE);
	gtk_widget_set_halign(p->buttongrid, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(p->buttongrid, TRUE);
	gtk_widget_set_valign(p->buttongrid, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		p->buttongrid, p->amount,
		GTK_POS_BOTTOM, 1, 1);

	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(p->dialog))), p->layout);

	return p;
}

void freePayDialog(PayDialog *p)
{
	gtk_widget_destroy(p->dialog);		// TODO does this destroy subwidgets?
	g_free(p);
}

gint runPayDialog(PayDialog *p)
{
	gtk_widget_show_all(p->dialog);
	return gtk_dialog_run(GTK_DIALOG(p->dialog));
}

gboolean payDialogAmountPaid(PayDialog *p, Price *pout)
{
	GtkWidget *alert;
	const char *amount;

	if (priceEntryGetPrice(PRICE_ENTRY(p->amount), pout) == TRUE)
		// TODO check less than p->price
		return TRUE;

	amount = priceEntryText(PRICE_ENTRY(p->amount));
	alert = gtk_message_dialog_new(GTK_WINDOW(p->dialog), GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
		"The value \"%s\" is not a valid amount paid.", amount);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(alert),
		"Please correct this amount paid and try again.");
	gtk_dialog_run(GTK_DIALOG(alert));
	gtk_widget_destroy(alert);
	return FALSE;
}
