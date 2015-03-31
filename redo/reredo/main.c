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
	if (argc == 2 && strcmp(argv[1], "manager") == 0)
		while (!manager())
			;
	else {
		OrderWindow *o;

		o = newOrderWindow();
		OrderWindowShow(o);
		gtk_main();
	}
	return 0;
}
