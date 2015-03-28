// 24 march 2015
#include "simplesale.h"

struct AddRemoveMoneyPriv {
};

#include "zaddremovemoney.h"

static void destroy(GtkWidget *widget, gpointer data)
{
	AddRemoveMoney *a = (AddRemoveMoney *) data;

	g_free(a);
}

GtkWidget *newAddRemoveMoney(void)
{
	AddRemoveMoney *a;

	a = makeAddRemoveMoneyFromUIFile();
	g_signal_connect(a->main, "destroy", G_CALLBACK(destroy), a);

	// Glade doesn't let me set these two
	gtk_button_set_image(GTK_BUTTON(a->addAmount), gtk_image_new_from_icon_name("list-add-symbolic", GTK_ICON_SIZE_MENU));
	gtk_button_set_image(GTK_BUTTON(a->removeAmount), gtk_image_new_from_icon_name("list-remove-symbolic", GTK_ICON_SIZE_MENU));

	return a->main;
}
