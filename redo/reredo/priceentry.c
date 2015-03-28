// 24 march 2015
#include "simplesale.h"

struct PriceEntryPriv {
	Price price;
	gboolean valid;
};

G_DEFINE_TYPE(PriceEntry, PriceEntry, GTK_TYPE_ENTRY)

static gboolean validate(const gchar *amount, Price *p)
{
	int n;
	int i;
	Price dollars = 0, cents = 0;

	n = g_utf8_strlen(amount, -1);
	if (n == 0)						// must enter something
		return FALSE;
	if (n == 1 && amount[0] == '.')		// just a . is invalid
		return FALSE;
	for (i = 0; i < n; i++) {
		if (amount[i] >= '0' && amount[i] <= '9') {
			dollars = dollars * 10 + (amount[i] - '0');
			continue;
		}
		if (amount[i] == '.')
			break;
		return FALSE;				// unrecognized character
	}
	if (i < n) {						// amount[i] == '.'
		i++;
		if (i == n)					// ###. is valid
			;
		else if (i != n - 2)			// must have either none or two digits
			return FALSE;
		else {
			cents = amount[i] - '0';
			cents = cents * 10 + (amount[i + 1] - '0');
		}
	}
	*p = (dollars * 100) + cents;
	return TRUE;
}

static void updateIconsAndNotifyValid(PriceEntry *p)
{
	char *icon;

	icon = NULL;
	if (!p->priv->valid)
		icon = "dialog-error";
	// TODO secondary?
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(p), GTK_ENTRY_ICON_PRIMARY, icon);
	g_object_notify(G_OBJECT(p), "valid");
}

// TODO switch to a vtable override?
static void changed(GtkEditable *e, gpointer data)
{
	PriceEntry *p = PriceEntry(e);
	Price price;

	p->priv->valid = validate(gtk_entry_get_text(GTK_ENTRY(p)), &price);
	// don't use g_object_set() here; the text is already in the entry field and we'll be calling updateIconsAndNotifyValid() anyway
	if (p->priv->valid)
		p->priv->price = price;
	updateIconsAndNotifyValid(p);
}

static void PriceEntry_init(PriceEntry *p)
{
	p->priv = G_TYPE_INSTANCE_GET_PRIVATE(p, PriceEntryType, struct PriceEntryPriv);
	gtk_entry_set_width_chars(GTK_ENTRY(p), 10);		// not too long, not too short
	gtk_entry_set_alignment(GTK_ENTRY(p), 1);
	g_signal_connect(p, "changed", G_CALLBACK(changed), NULL);
	// and get the ball rolling
	g_object_set(p, "price", 0, NULL);
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

static GParamSpec *priceEntryProperties[3];

static void PriceEntry_set_property(GObject *obj, guint prop, const GValue *value, GParamSpec *spec)
{
	PriceEntry *p = PriceEntry(obj);

	if (prop != 1) {		// only price is settable
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, spec);
		return;
	}
	p->priv->price = g_value_get_Price(value);
	p->priv->valid = TRUE;
	// TODO will this call our changed handler?
	// TODO get rid of the + 1 kludge
	// TODO this leaks memory
	gtk_entry_set_text(GTK_ENTRY(p), priceString(p->priv->price) + 1);
	updateIconsAndNotifyValid(p);
}

static void PriceEntry_get_property(GObject *obj, guint prop, GValue *value, GParamSpec *spec)
{
	PriceEntry *p = PriceEntry(obj);

	switch (prop) {
	case 1:		// price
		g_value_set_Price(value, p->priv->price);
		return;
	case 2:		// valid
		g_value_set_boolean(value, p->priv->valid);
		return;
	}
	G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, spec);
}

static void PriceEntry_class_init(PriceEntryClass *class)
{
	g_type_class_add_private(class, sizeof (struct PriceEntryPriv));

	G_OBJECT_CLASS(class)->dispose = PriceEntry_dispose;
	G_OBJECT_CLASS(class)->finalize = PriceEntry_finalize;
	G_OBJECT_CLASS(class)->set_property = PriceEntry_set_property;
	G_OBJECT_CLASS(class)->get_property = PriceEntry_get_property;

	priceEntryProperties[1] = PriceParamSpec(
		"price",
		"Price",
		"Price",
		PriceMin, PriceMax,
		0,
		G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
	priceEntryProperties[2] = g_param_spec_boolean(
		"valid",
		"Valid",
		"Valid",
		FALSE,
		G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
g_object_class_install_properties(G_OBJECT_CLASS(class), 3, priceEntryProperties);
}
