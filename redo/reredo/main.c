// 28 march 2015
#include "simplesale.h"

// TODO eliminate the need for this
static void initTypes(void)
{
	PriceEntry_get_type();
	PriceRenderer_get_type();
}

typedef void (*initp)(void);

static const initp initFunctions[] = {
	initTypes,
	NULL,
};

int main(int argc, char *argv[])
{
	initp *init;
	extern void orderwindow(void);

	gtk_init(NULL, NULL);		// TODO args?
	for (init = initFunctions; *init != NULL; init++)
		(*(*init))();
	backend = Backend(g_object_new(MockBackendType, NULL));
//TODO	gtk_main();
	if (argc == 2 && strcmp(argv[1], "manager") == 0) {
		Manager *m;

		m = newManager();
		gtk_widget_show_all(GTK_WIDGET(m));
		gtk_main();
	} else if (argc == 2 && strcmp(argv[1], "order") == 0) {
		OrderWindow *o;

		o = newOrderWindow();
		gtk_widget_show_all(GTK_WIDGET(o));
		gtk_main();
	} else {
		GtkWidget *widget;

		widget = newPayDialog();
		gtk_widget_show_all(widget);
		gtk_main();
	}
	return 0;
}
