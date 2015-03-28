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

int main(void)
{
	initp *init;

	gtk_init(NULL, NULL);		// TODO args?
	for (init = initFunctions; *init != NULL; init++)
		(*(*init))();
	backend = Backend(g_object_new(MockBackendType, NULL));
//TODO	gtk_main();
	while (!manager())
		;
	return 0;
}
