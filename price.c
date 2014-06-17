// 16 june 2014
#include "simplesale.h"

gchar *priceToString(Price p, char *prefix)
{
	Price dollars, cents;

	dollars = p / 100;
	cents = p % 100;
	return g_strdup_printf("%s%" PRICEFMT ".%02" PRICEFMT, prefix, dollars, cents);
}

// PriceEntry is a special GtkEntry that only allows prices to be entered.

static void priceEntry_initEditable(GtkEditableInterface *);

G_DEFINE_TYPE_WITH_CODE(PriceEntry, priceEntry, GTK_TYPE_ENTRY,
	G_IMPLEMENT_INTERFACE(GTK_TYPE_EDITABLE, priceEntry_initEditable))

static void priceEntry_init(PriceEntry *p)
{
	gtk_entry_set_width_chars(GTK_ENTRY(p), 8);
	gtk_entry_set_alignment(GTK_ENTRY(p), 1);
}

static void priceEntry_dispose(GObject *obj)
{
	G_OBJECT_CLASS(priceEntry_parent_class)->dispose(obj);
}

static void priceEntry_finalize(GObject *obj)
{
	G_OBJECT_CLASS(priceEntry_parent_class)->finalize(obj);
}

static GtkEditableInterface *chain = NULL;

static void priceEntry_insertText(GtkEditable *editable, const gchar *text, gint n, gint *pos)
{
	gint i;

	for (i = 0; i < n; i++) {
		if (text[i] >= '0' && text[i] <= '9')
			continue;
		if (text[i] == '.')
			continue;
		// bad character; beep and reject edit
		gtk_widget_error_bell(GTK_WIDGET(editable));
		return;
	}
	// otherwise let the text in
	chain->insert_text(editable, text, n, pos);
}

static void priceEntry_initEditable(GtkEditableInterface *interface)
{
	chain = (GtkEditableInterface *) g_type_interface_peek_parent(interface);
#define CHAIN(x) interface->x = chain->x;
	// these came from gtk/gtkeditable.h because the docs don't list this interface??? TODO
	// signals
	interface->insert_text = priceEntry_insertText;
	CHAIN(delete_text)
	CHAIN(changed)
	// vtable
	CHAIN(do_insert_text)
	CHAIN(do_delete_text)
	CHAIN(get_chars)
	CHAIN(set_selection_bounds)
	CHAIN(get_selection_bounds)
	CHAIN(set_position)
	CHAIN(get_position)
#undef CHAIN
}

static void priceEntry_class_init(PriceEntryClass *class)
{
	G_OBJECT_CLASS(class)->dispose = priceEntry_dispose;
	G_OBJECT_CLASS(class)->finalize = priceEntry_finalize;
}

GtkWidget *newPriceEntry(void)
{
	return (GtkWidget *) g_object_new(priceEntry_get_type(), NULL);
}

int priceEntryGetPrice(PriceEntry *p, Price *pout)
{
	const gchar *amount;
	Price dollars = 0, cents = 0;
	gint i, n;

	amount = gtk_entry_get_text(GTK_ENTRY(p));
	if (amount == NULL)				// must enter something
		return priceEntryEmpty;
	n = strlen(amount);
	if (n == 0)						// must enter something
		return priceEntryEmpty;
	if (n == 1 && amount[0] == '.')		// just a . is invalid
		return priceEntryInvalid;
	for (i = 0; i < n; i++) {
		if (amount[i] >= '0' && amount[i] <= '9') {
			dollars = dollars * 10 + (amount[i] - '0');
			continue;
		}
		if (amount[i] == '.')
			break;
		return priceEntryInvalid;		// unrecognized character
	}
	if (i < n) {				// amount[i] == '.'
		i++;
		if (i == n)			// ###. is valid
			;
		else if (i != n - 2)	// must have either none or two digits
			return priceEntryInvalid;
		else {
			cents = amount[i] - '0';
			cents = cents * 10 + (amount[i + 1] - '0');
		}
	}
	*pout = PRICE(dollars, cents);
	return priceEntryOK;
}

// PriceRenderer is a special GtkCellRendererText that renders prices.

struct PriceRenderer {
	GtkCellRendererText parent_instance;

	Price price;
	char *prefix;
};

typedef struct PriceRendererClass PriceRendererClass;

struct PriceRendererClass {
	GtkCellRendererTextClass parent_class;
};

G_DEFINE_TYPE(PriceRenderer, priceRenderer, GTK_TYPE_CELL_RENDERER_TEXT)

static void priceRenderer_init(PriceRenderer *p)
{
	USED(p);
}

static void priceRenderer_dispose(GObject *obj)
{
	G_OBJECT_CLASS(priceRenderer_parent_class)->dispose(obj);
}

static void priceRenderer_finalize(GObject *obj)
{
	PriceRenderer *p = (PriceRenderer *) obj;

	g_free(p->prefix);
	G_OBJECT_CLASS(priceRenderer_parent_class)->finalize(obj);
}

static GParamSpec *priceRendererProperties[3];

static void priceRenderer_setProperty(GObject *obj, guint id, const GValue *value, GParamSpec *pspec)
{
	PriceRenderer *p = (PriceRenderer *) obj;
	char *newText;

	if (id == 1)			// price
		p->price = PRICEVALUE(value);
	else if (id == 2) {		// prefix
		g_free(p->prefix);
		p->prefix = g_value_dup_string(value);
	} else {				// something else
		G_OBJECT_WARN_INVALID_PROPERTY_ID(p, id, pspec);
		return;
	}
	newText = priceToString(p->price, p->prefix);
	g_object_set(p, "text", newText, NULL);
	g_free(newText);
}

static void priceRenderer_getProperty(GObject *obj, guint id, GValue *value, GParamSpec *pspec)
{
	USED(value);

	G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, id, pspec);
}

static void priceRenderer_class_init(PriceRendererClass *class)
{
	G_OBJECT_CLASS(class)->dispose = priceRenderer_dispose;
	G_OBJECT_CLASS(class)->finalize = priceRenderer_finalize;
	G_OBJECT_CLASS(class)->set_property = priceRenderer_setProperty;
	G_OBJECT_CLASS(class)->get_property = priceRenderer_getProperty;

	priceRendererProperties[1] = PRICEPARAMSPEC(
		"price",
		"Price",
		"Price",
		PRICEMIN, PRICEMAX,
		0,
		G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);
	priceRendererProperties[2] = g_param_spec_string(
		"prefix",
		"Prefix",
		"Prefix for monetary values, usually $.",
		"$",
		G_PARAM_WRITABLE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);
	g_object_class_install_properties(G_OBJECT_CLASS(class), 3, priceRendererProperties);
}

GtkCellRenderer *newPriceRenderer(void)
{
	return (GtkCellRenderer *) g_object_new(priceRenderer_get_type(), NULL);
}
