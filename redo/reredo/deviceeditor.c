// 24 march 2015
#include "simplesale.h"

struct DeviceEditorPriv {
};

#include "zdeviceeditor.h"

static void destroy(GtkWidget *widget, gpointer data)
{
	DeviceEditor *d = (DeviceEditor *) data;

	g_free(d);
}

GtkWidget *newDeviceEditor(void)
{
	DeviceEditor *d;

	d = makeDeviceEditorFromUIFile();
	g_signal_connect(d->main, "destroy", G_CALLBACK(destroy), d);

	return d->main;
}
