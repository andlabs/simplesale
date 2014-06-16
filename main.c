// 14 june 2014
#include "simplesale.h"

static gchar *theme = NULL;
static GOptionEntry flags[] = {
	{ "theme", 0, 0, G_OPTION_ARG_STRING, &theme, "if set, GTK+ theme to use", "THEME" },
	{ NULL, 0, 0, 0, NULL, NULL, NULL },
};

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

	initAccounts();
	initItems();
	addAccount("Test Owner", "abcd");
	addAccount("Test Cashier", "efgh");
	newLogin();
	newShift("Test Cashier");
	gtk_main();
	return 0;

usage:
	fprintf(stderr, "%s\n", g_option_context_get_help(flagscontext, FALSE, NULL));
	return 1;
}
