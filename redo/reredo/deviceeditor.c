// 24 march 2015
#include "simplesale.h"

#define DeviceEditorType (DeviceEditor_get_type())
#define DeviceEditor(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), DeviceEditorType, DeviceEditor))
#define IsDeviceEditor(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), DeviceEditorType))
#define DeviceEditorClass(class) (G_TYPE_CHECK_CLASS_CAST((class), DeviceEditorType, DeviceEditorClass))
#define IsDeviceEditorClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), DeviceEditor))
#define GetDeviceEditorClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), DeviceEditorType, DeviceEditorClass))
typedef struct DeviceEditor DeviceEditor;
typedef struct DeviceEditorClass DeviceEditorClass;
struct DeviceEditor {
	GtkBox parent_instance;
	struct DeviceEditorPrivate *priv;
};
struct DeviceEditorClass {
	GtkBoxClass parent_class;
};
static GType DeviceEditor_get_type(void);

typedef struct DeviceEditorPrivate DeviceEditorPrivate;

struct DeviceEditorPrivate {
	GtkWidget *receiptList;
	GtkWidget *kitchenList;
	GtkWidget *receiptTest;
	GtkWidget *kitchenTest;
};

G_DEFINE_TYPE_WITH_CODE(DeviceEditor, DeviceEditor, GTK_TYPE_BOX,
	G_ADD_PRIVATE(DeviceEditor))

static void DeviceEditor_init(DeviceEditor *d)
{
	d->priv = DeviceEditor_get_instance_private(d);

	gtk_widget_init_template(GTK_WIDGET(d));
}

static void DeviceEditor_dispose(GObject *obj)
{
	G_OBJECT_CLASS(DeviceEditor_parent_class)->dispose(obj);
}

static void DeviceEditor_finalize(GObject *obj)
{
	G_OBJECT_CLASS(DeviceEditor_parent_class)->finalize(obj);
}

static void DeviceEditor_class_init(DeviceEditorClass *class)
{
	G_OBJECT_CLASS(class)->dispose = DeviceEditor_dispose;
	G_OBJECT_CLASS(class)->finalize = DeviceEditor_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/deviceeditor.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), DeviceEditor, name)
	T(receiptList);
	T(kitchenList);
	T(receiptTest);
	T(kitchenTest);
#undef T
}

GtkWidget *newDeviceEditor(void)
{
	return GTK_WIDGET(g_object_new(DeviceEditorType, NULL));
}
