// 28 march 2015
#include "simplesale.h"

// TODO
// - https://git.gnome.org/browse/gedit/tree/gedit/resources/css/gedit-style.css
// - rename OrderWindowUI to OrderWindowGUI?

struct OrderWindowUIPriv {
};

#include "zorderwindow.h"

void orderwindow(void)
{
	OrderWindowUI *o;

	o = makeOrderWindowUIFromUIFile();
	// TODO grow the window

	setName0Price1TreeViewColumnLayout(GTK_TREE_VIEW(o->orderList));

	BackendSetItemsIconView(backend, GTK_ICON_VIEW(o->items));

	gtk_widget_show_all(o->main);
	gtk_main();
}
