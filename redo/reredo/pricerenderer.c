// 27 march 2015
#include "simplesale.h"

struct PriceRendererPriv {
	Price price;
};

G_DEFINE_TYPE(PriceRenderer, PriceRenderer, GTK_TYPE_CELL_RENDERER_TEXT)

static void PriceRenderer_init(PriceRenderer *p)
{
	p->priv = G_TYPE_INSTANCE_GET_PRIVATE(p, PriceRendererType, struct PriceRendererPriv);
	g_object_set(p, "price", 0, NULL);
}

static void PriceRenderer_dispose(GObject *obj)
{
	PriceRenderer *p = PriceRenderer(obj);

	G_OBJECT_CLASS(PriceRenderer_parent_class)->dispose(obj);
}

static GParamSpec *priceRendererProperties[2];

static void PriceRenderer_finalize(GObject *obj)
{
	G_OBJECT_CLASS(PriceRenderer_parent_class)->finalize(obj);
}

static void PriceRenderer_set_property(GObject *obj, guint prop, const GValue *value, GParamSpec *spec)
{
	PriceRenderer *p = PriceRenderer(obj);
	char *newText;

	if (prop != 1) {		// not price
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, spec);
		return;
	}
	p->priv->price = g_value_get_Price(value);
	newText = priceString(p->priv->price);
	g_object_set(p, "text", newText, NULL);
	g_free(newText);
}

static void PriceRenderer_get_property(GObject *obj, guint prop, GValue *value, GParamSpec *spec)
{
//TODO	PriceRenderer *p = PriceRenderer(obj);

//TODO	if (id != 1) {		// not price
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, spec);
//TODO		return;
//TODO	}
}

static void PriceRenderer_class_init(PriceRendererClass *class)
{
	g_type_class_add_private(class, sizeof (struct PriceRendererPriv));

	G_OBJECT_CLASS(class)->dispose = PriceRenderer_dispose;
	G_OBJECT_CLASS(class)->finalize = PriceRenderer_finalize;
	G_OBJECT_CLASS(class)->set_property = PriceRenderer_set_property;
	G_OBJECT_CLASS(class)->get_property = PriceRenderer_get_property;

	priceRendererProperties[1] = PriceParamSpec(
		"price",
		"Price",
		"Price",
		PriceMin, PriceMax,
		0,
		G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);
	g_object_class_install_properties(G_OBJECT_CLASS(class), 2, priceRendererProperties);
}

GtkCellRenderer *newPriceRenderer(void)
{
	return GTK_CELL_RENDERER(g_object_new(PriceRendererType, NULL));
}
