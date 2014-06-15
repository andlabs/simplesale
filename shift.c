// 15 june 2014
#include "simplesale.h"

struct shift {
	char *employee;
	GHashTable *orders;
};

shift *newShift(char *name)
{
	shift *s;

	s = (shift *) g_malloc0(sizeof (shift));
	s->employee = g_strdup(name);
	s->orders = g_hash_table_new(g_direct_hash, g_direct_equal);
	return s;
}

void shiftNewOrder(shift *s)
{
	order *o;
	orderWindow *ow;

	o = newOrder();
	// sample items
	{
		addItem("Regular Slice", PRICE(2, 00));
		addItem("Large Soda", PRICE(1, 50));
		addItem("Cookie", PRICE(1, 00));
		addToOrder(o, 0);
		addToOrder(o, 1);
	}
	ow = newOrderWindow(s, o);
	g_hash_table_insert(s->orders, o, ow);
}

void shiftDoOrder(shift *s, order *o, int action)
{
	USED(s);
	// TODO get out orderWindow
	USED(o);
	switch (action) {
	case orderCancel:
		printf("order cancelled\n");
		break;
	case orderPayNow:
		printf("order paid now\n");
		break;
	case orderPayLater:
		printf("order paid later\n");
		break;
	}
	gtk_main_quit();
}
