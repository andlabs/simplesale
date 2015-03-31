// 30 march 2015
#include "simplesale.h"

struct Order {
	char *name;
	// TODO order type
	GtkListStore *order;
	Price subtotal;
};

Order *newOrder(void)
{
	Order *o;

	o = g_new0(Order, 1);
	o->order = gtk_list_store_new(2,
		G_TYPE_STRING,		// item name
		PriceType);			// item price
	return o;
}

void deleteOrder(Order *o)
{
	g_free(o->name);
	g_object_unref(o->order);
	g_free(o);
}

// TODO const char *?
char *OrderCustomerName(Order *o)
{
	return o->name;
}

// TODO const char *?
void OrderSetCustomerName(Order *o, char *name)
{
	// TODO make sure this works with NULL (initial o->name)
	g_free(o->name);
	o->name = g_strdup(name);
}

void OrderSetTreeView(Order *o, GtkTreeView *tv)
{
	gtk_tree_view_set_model(tv, GTK_TREE_MODEL(o->order));
	setName0Price1TreeViewColumnLayout(tv);
}

void OrderAppendItem(Order *o, GtkTreeIter *itemIter)
{
	char *name;
	Price price;
	GtkTreeIter iter;

	// TODO provide one function to get both?
	name = BackendItemName(backend, itemIter);
	price = BackendItemPrice(backend, itemIter);
	gtk_list_store_append(o->order, &iter);
	gtk_list_store_set(o->order, &iter, 0, name, 1, price, -1);
	o->subtotal += price;
	// TODO upate titles here?
}

void OrderAppendNote(Order *o, char *note)
{
	char *xnote;
	GtkTreeIter iter;

	// TODO configurable?
	xnote = g_strdup_printf(" - %s", note);
	gtk_list_store_append(o->order, &iter);
	gtk_list_store_set(o->order, &iter, 0, xnote, 1, 0, -1);
	g_free(xnote);
}

void OrderRemoveItem(Order *o, GtkTreeIter *iter)
{
	Price price;

	gtk_tree_model_get(GTK_TREE_MODEL(o->order), iter, 1, &price, -1);
	gtk_list_store_remove(o->order, iter);
	o->subtotal -= price;
	// TODO update labels here?
}

Price OrderTotal(Order *o)
{
	// TODO
	return o->subtotal;
}

Price OrderSubtotal(Order *o)
{
	return o->subtotal;
}
