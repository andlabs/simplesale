// 28 march 2015
#include "simplesale.h"

// TODO
// - https://git.gnome.org/browse/gedit/tree/gedit/resources/css/gedit-style.css
// - rename OrderWindowUI to OrderWindowGUI?

struct OrderWindowUIPriv {
	GtkWidget *removeItem;
};

#include "zorderwindow.h"

void orderwindow(void)
{
	OrderWindowUI *o;
	GtkWidget *actionBar;
	GtkWidget *addNoteButton;

	o = makeOrderWindowUIFromUIFile();
	// TODO grow the window

	// bind header to body; that's more likely to be set properly
	g_object_bind_property(o->headerPaned, "position",
		o->bodyPaned, "position",
		G_BINDING_DEFAULT | G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);

	// TODO double-check to make sure Glade doesn't have this
	actionBar = gtk_action_bar_new();
	o->priv->removeItem = gtk_button_new_with_label("Remove Item");
	gtk_action_bar_pack_start(GTK_ACTION_BAR(actionBar), o->priv->removeItem);
	addNoteButton = gtk_button_new_with_label("Add Note");
	gtk_action_bar_pack_end(GTK_ACTION_BAR(actionBar), addNoteButton);
	gtk_container_add(GTK_CONTAINER(o->box1), actionBar);

	setName0Price1TreeViewColumnLayout(GTK_TREE_VIEW(o->orderList));

	BackendSetItemsIconView(backend, GTK_ICON_VIEW(o->items));

	gtk_widget_show_all(o->main);
	gtk_main();
}
