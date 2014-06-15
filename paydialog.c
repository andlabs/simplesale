// 15 june 2014
#include "simplesale.h"

struct payDialog {
	GtkWidget *dialog;
	GtkWidget *layout;
	GtkWidget *amount;
	GtkWidget *buttongrid;
	GtkWidget *digits[10];
	GtkWidget *decimal;
};

static void stripInvalidChars(GtkEditable *editable, gchar *text, gint n, gpointer pos, gpointer data)
{
	USED(pos);		// don't validate here; we do that after the user is done

	payDialog *p = (payDialog *) data;
	gint i;

	for (i = 0; i < n; i++) {
		if (text[i] >= '0' && text[i] <= '9')
			continue;
		if (text[i] == '.')
			continue;
		gtk_widget_error_bell(p->amount);
		g_signal_stop_emission_by_name(editable, "insert-text");
		return;
	}
	// otherwise let the text in
}

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
	// TODO set primary/secondary buttons

	p->layout = gtk_grid_new();

	label = gtk_label_new("Order Total:");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, NULL,
		GTK_POS_TOP, 1, 1);
	firstlabel = label;
{GtkWidget *xl;
	label = gtk_label_new("$");
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, firstlabel,
		GTK_POS_RIGHT, 1, 1);
xl = label;
	// TODO total not subtotal
	pricestr = priceToString(subtotal(o), "");
	label = gtk_label_new(pricestr);
	g_free(pricestr);
	alignLabel(label, 1);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
//		label, firstlabel,
label,xl,
		GTK_POS_RIGHT, 1, 1);
}
	label = gtk_label_new("Amount Given:");
	alignLabel(label, 1);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, firstlabel,
		GTK_POS_BOTTOM, 1, 1);
	firstlabel = label;
	label = gtk_label_new("$");
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		label, firstlabel,
		GTK_POS_RIGHT, 1, 1);
	firstlabel = label;		// for button grid
	p->amount = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(p->amount), 8);
	gtk_entry_set_alignment(GTK_ENTRY(p->amount), 1);
	g_signal_connect(p->amount, "insert-text", G_CALLBACK(stripInvalidChars), p);
	// TODO restrict input to numeric
	gtk_widget_set_hexpand(p->amount, TRUE);
	gtk_widget_set_halign(p->amount, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		p->amount, label,
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
	gtk_widget_set_hexpand(p->buttongrid, TRUE);
	gtk_widget_set_halign(p->buttongrid, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(p->buttongrid, TRUE);
	gtk_widget_set_valign(p->buttongrid, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(p->layout),
		p->buttongrid, firstlabel,
		GTK_POS_BOTTOM, 2, 1);

	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(p->dialog))), p->layout);

	return p;
}

void freePayDialog(payDialog *p)
{
	gtk_widget_destroy(p->dialog);		// TODO does this destroy subwidgets?
	g_free(p);
}

gint runPayDialog(payDialog *p)
{
	gtk_widget_show_all(p->dialog);
	return gtk_dialog_run(GTK_DIALOG(p->dialog));
}

gboolean payDialogAmountPaid(payDialog *p, price *pout)
{
	const gchar *amount;
	price dollars = 0, cents = 0;
	gint i, n;
	GtkWidget *alert;

	amount = gtk_entry_get_text(GTK_ENTRY(p->amount));
	if (amount == NULL)				// must enter something
		goto empty;
	n = strlen(amount);
	if (n == 0)						// must enter something
		goto empty;
	if (n == 1 && amount[0] == '.')		// just a . is invalid
		goto bad;
	for (i = 0; i < n; i++) {
		if (amount[i] >= '0' && amount[i] <= '9') {
			dollars = dollars * 10 + (amount[i] - '0');
			continue;
		}
		if (amount[i] == '.')
			break;
		goto bad;			// unrecognized character
	}
	if (i < n) {				// amount[i] == '.'
		i++;
		if (i == n)			// ###. is valid
			;
		else if (i != n - 2)	// must have either none or two digits
			goto bad;
		else {
			cents = amount[i] - '0';
			cents = cents * 10 + (amount[i + 1] - '0');
		}
	}
	*pout = PRICE(dollars, cents);
	return TRUE;

bad:
	alert = gtk_message_dialog_new(GTK_WINDOW(p->dialog), GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
		"The value \"%s\" is not a valid amount paid.", amount);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(alert),
		"Please correct this amount paid and try again.");
	gtk_dialog_run(GTK_DIALOG(alert));
	gtk_widget_destroy(alert);
	return FALSE;

empty:
	alert = gtk_message_dialog_new(GTK_WINDOW(p->dialog), GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
		"You must specify an amount paid.");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(alert),
		"Please enter an amount paid and try again.");
	gtk_dialog_run(GTK_DIALOG(alert));
	gtk_widget_destroy(alert);
	return FALSE;
}
