// 24 march 2015
#include "simplesale.h"

#include "zothersettings.h"

static void destroy(GtkWidget *widget, gpointer data)
{
	OtherSettings *o = (OtherSettings *) data;

	g_free(o);
}

GtkWidget *newOtherSettings(void)
{
	OtherSettings *o;

	o = makeOtherSettingsFromUIFile();
	g_signal_connect(o->main, "destroy", G_CALLBACK(destroy), o);

	return o->main;
}
