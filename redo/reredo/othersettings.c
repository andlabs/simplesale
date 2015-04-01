// 24 march 2015
#include "simplesale.h"

#define OtherSettingsType (OtherSettings_get_type())
#define OtherSettings(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), OtherSettingsType, OtherSettings))
#define IsOtherSettings(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), OtherSettingsType))
#define OtherSettingsClass(class) (G_TYPE_CHECK_CLASS_CAST((class), OtherSettingsType, OtherSettingsClass))
#define IsOtherSettingsClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), OtherSettings))
#define GetOtherSettingsClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), OtherSettingsType, OtherSettingsClass))
typedef struct OtherSettings OtherSettings;
typedef struct OtherSettingsClass OtherSettingsClass;
struct OtherSettings {
	GtkGrid parent_instance;
	struct OtherSettingsPrivate *priv;
};
struct OtherSettingsClass {
	GtkGridClass parent_class;
};
static GType OtherSettings_get_type(void);

typedef struct OtherSettingsPrivate OtherSettingsPrivate;

struct OtherSettingsPrivate {
	GtkWidget *tax;
	GtkWidget *changeManagerPassword;
};

G_DEFINE_TYPE_WITH_CODE(OtherSettings, OtherSettings, GTK_TYPE_GRID,
	G_ADD_PRIVATE(OtherSettings))

static void OtherSettings_init(OtherSettings *o)
{
	o->priv = OtherSettings_get_instance_private(o);

	gtk_widget_init_template(GTK_WIDGET(o));
}

static void OtherSettings_dispose(GObject *obj)
{
	G_OBJECT_CLASS(OtherSettings_parent_class)->dispose(obj);
}

static void OtherSettings_finalize(GObject *obj)
{
	G_OBJECT_CLASS(OtherSettings_parent_class)->finalize(obj);
}

static void OtherSettings_class_init(OtherSettingsClass *class)
{
	G_OBJECT_CLASS(class)->dispose = OtherSettings_dispose;
	G_OBJECT_CLASS(class)->finalize = OtherSettings_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/othersettings.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), OtherSettings, name)
	T(tax);
	T(changeManagerPassword);
#undef T
}

GtkWidget *newOtherSettings(void)
{
	return GTK_WIDGET(g_object_new(OtherSettingsType, NULL));
}
