// 24 march 2015
#include "simplesale.h"

struct PriceEntryPriv {
	Price p;
};

G_DEFINE_TYPE(PriceEntry, PriceEntry, GTK_TYPE_ENTRY)

static void PriceEntry_init(PriceEntry *p)
{
	p->priv = G_TYPE_INSTANCE_GET_PRIVATE(p, PriceEntryType, struct PriceEntryPriv);
}

static void PriceEntry_dispose(GObject *obj)
{
	PriceEntry *p = PriceEntry(obj);

	G_OBJECT_CLASS(PriceEntry_parent_class)->dispose(obj);
}

static void PriceEntry_finalize(GObject *obj)
{
	G_OBJECT_CLASS(PriceEntry_parent_class)->finalize(obj);
}

static void PriceEntry_set_property(GObject *obj, guint prop, const GValue *value, GParamSpec *spec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, spec);
}

static void PriceEntry_get_property(GObject *obj, guint prop, GValue *value, GParamSpec *spec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, spec);
}

static void PriceEntry_class_init(PriceEntryClass *class)
{
	g_type_class_add_private(class, sizeof (struct PriceEntryPriv));

	G_OBJECT_CLASS(class)->dispose = PriceEntry_dispose;
	G_OBJECT_CLASS(class)->finalize = PriceEntry_finalize;
	G_OBJECT_CLASS(class)->set_property = PriceEntry_set_property;
	G_OBJECT_CLASS(class)->get_property = PriceEntry_get_property;
}
