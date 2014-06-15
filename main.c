// 14 june 2014
#include "simplesale.h"

static gchar *theme = NULL;
static GOptionEntry flags[] = {
	{ "theme", 0, 0, G_OPTION_ARG_STRING, &theme, "if set, GTK+ theme to use", "THEME" },
	{ NULL, 0, 0, 0, NULL, NULL, NULL },
};

typedef struct controller controller;

struct controller {
	GHashTable *orders;
};

static controller *c;

static void initController(void)
{
	c = (controller *) g_malloc0(sizeof (controller));
	c->orders = g_hash_table_new(g_direct_hash, g_direct_equal);
}

void scNewOrder(void)
{
	order *o;
	orderWindow *ow;

	o = newOrder();
	ow = newOrderWindow(o);
	// sample items
	{
		addItem("Regular Slice", PRICE(2, 00));
		addItem("Large Soda", PRICE(1, 50));
		addItem("Cookie", PRICE(1, 00));
		addToOrder(o, 0);
		addToOrder(o, 1);
	}
	g_hash_table_insert(c->orders, o, ow);
}

void scDoOrder(order *o, int action)
{
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

int main(int argc, char *argv[])
{
	GOptionContext *flagscontext;
	GError *flagserr = NULL;

	flagscontext = g_option_context_new("extra text here TODO");
	g_option_context_add_main_entries(flagscontext, flags, NULL);
	g_option_context_add_group(flagscontext, gtk_get_option_group(TRUE));		// also initializes GTK+
	if (g_option_context_parse(flagscontext, &argc, &argv, &flagserr) != TRUE) {
		fprintf(stderr, "error: %s\n", flagserr->message);
		goto usage;
	}

	if (theme != NULL)
		g_object_set(gtk_settings_get_default(),
			"gtk-theme-name", theme,
			NULL);

	initController();
	initItems();
	scNewOrder();
	gtk_main();
	return 0;

usage:
	fprintf(stderr, "%s\n", g_option_context_get_help(flagscontext, FALSE, NULL));
	return 1;
}
