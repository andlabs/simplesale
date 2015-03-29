// 28 march 2015
#include "simplesale.h"

struct OrderWindowPriv {
	GtkWidget *removeItem;
};

#include "zorderwindow.h"

void orderwindow(void)
{
	OrderWindow *o;
	GtkWidget *actionBar;
	GtkWidget *addNoteButton;

	o = makeOrderWindowFromUIFile();

	// bind header to body; that's more likely to be set properly
	g_object_bind_property(o->headerPaned, "position",
		o->bodyPaned, "position",
		G_BINDING_DEFAULT | G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);

	gtk_widget_show_all(o->main);
	gtk_main();
}
