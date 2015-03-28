// 24 march 2015
#include "simplesale.h"

// TODO selection changed does not fire when deleting the last item (which selects the new last item)...?
// or rather, selectedIter doesn't change?

struct ItemEditorPriv {
	gboolean selected;
	GtkTreeIter selectedIter;		// TODO rename selection?
	gulong nameChangedHandler;
	gulong priceChangedHandler;
};

#include "zitemeditor.h"

static void destroy(GtkWidget *widget, gpointer data)
{
	ItemEditor *e = (ItemEditor *) data;

	g_free(e);
}

static void nameChanged(GtkEditable *editable, gpointer data)
{
	ItemEditor *e = (ItemEditor *) data;

	if (!e->priv->selected)
		g_error("item name changed with no item selected");
	BackendSetItemName(backend, &(e->priv->selectedIter), gtk_entry_get_text(GTK_ENTRY(e->name)));
}

static void priceChanged(GtkEditable *editable, gpointer data)
{
	ItemEditor *e = (ItemEditor *) data;
	gboolean valid;
	Price price;

	if (!e->priv->selected)
		g_error("item name changed with no item selected");
	g_object_get(e->price,
		"valid", &valid,
		"price", &price,
		NULL);
	if (valid)
		BackendSetItemPrice(backend, &(e->priv->selectedIter), price);
}

static void selectionChanged(GtkTreeSelection *sel, gpointer data)
{
	ItemEditor *e = (ItemEditor *) data;

	e->priv->selected = gtk_tree_selection_get_selected(e->listSelection, NULL, &(e->priv->selectedIter));
	gtk_widget_set_sensitive(e->name, e->priv->selected);
	gtk_widget_set_sensitive(e->price, e->priv->selected);
	gtk_widget_set_sensitive(e->removeButton, e->priv->selected);
	if (e->priv->selected) {
		// TODO provide a method to get both at once?
		gtk_entry_set_text(GTK_ENTRY(e->name), BackendItemName(backend, &(e->priv->selectedIter)));
		g_object_set(e->price, "price", BackendItemPrice(backend, &(e->priv->selectedIter)), NULL);
	} else {
		// clear the fields to make deselection make sense
		// we need to disable our signal handlers to prevent the g_error() calls in our signal handlers from firing
		// TODO really necessary?
		g_signal_handler_block(e->name, e->priv->nameChangedHandler);
		g_signal_handler_block(e->price, e->priv->priceChangedHandler);
		gtk_entry_set_text(GTK_ENTRY(e->name), "");
		g_object_set(e->price, "price", 0, NULL);
		g_signal_handler_unblock(e->name, e->priv->nameChangedHandler);
		g_signal_handler_unblock(e->price, e->priv->priceChangedHandler);
	}
}

static void addItem(GtkToolButton *b, gpointer data)
{
	BackendAppendItem(backend, "New Item", 0);
}

static void removeItem(GtkToolButton *b, gpointer data)
{
	ItemEditor *e = (ItemEditor *) data;

	if (!e->priv->selected)
		g_error("remove item button clicked without any item selected");
	BackendDeleteItem(backend, &(e->priv->selectedIter));
}

GtkWidget *newItemEditor(void)
{
	ItemEditor *e;

	e = makeItemEditorFromUIFile();
	g_signal_connect(e->main, "destroy", G_CALLBACK(destroy), e);

	BackendSetItemsTreeView(backend, GTK_TREE_VIEW(e->list));
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(e->list), GTK_ENTRY(e->search));
	// TODO search should do case-insensitive substring or word-string match

	e->priv->nameChangedHandler = g_signal_connect(e->name, "changed", G_CALLBACK(nameChanged), e);
	// TODO signal on valid instead?
	e->priv->priceChangedHandler = g_signal_connect(e->price, "changed", G_CALLBACK(priceChanged), e);
	g_signal_connect(e->listSelection, "changed", G_CALLBACK(selectionChanged), e);
	g_signal_connect(e->addButton, "clicked", G_CALLBACK(addItem), e);
	g_signal_connect(e->removeButton, "clicked", G_CALLBACK(removeItem), e);

	// get the ball rolling
	// TODO make this unnecessary
	// TODO temporarily disabled due to PriceEntry GtkBuilder bug
//TODO	selectionChanged(e->listSelection, e);

	return e->main;
}
