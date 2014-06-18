// 16 june 2014
#include "simplesale.h"

gchar *priceToString(Price p, char *prefix)
{
	Price dollars, cents;

	dollars = p / 100;
	cents = p % 100;
	return g_strdup_printf("%s%" PRICEFMT ".%02" PRICEFMT, prefix, dollars, cents);
}

// RealPriceEntry is a special GtkEntry that only allows prices to be entered.

#define REAL_PRICE_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), realPriceEntry_get_type(), RealPriceEntry))

typedef struct RealPriceEntry RealPriceEntry;
typedef struct RealPriceEntryClass RealPriceEntryClass;

struct RealPriceEntry {
	GtkEntry parent_instance;

	Price price;
	gboolean valid;
};

struct RealPriceEntryClass {
	GtkEntryClass parent_class;
};

static void realPriceEntry_initEditable(GtkEditableInterface *);

G_DEFINE_TYPE_WITH_CODE(RealPriceEntry, realPriceEntry, GTK_TYPE_ENTRY,
	G_IMPLEMENT_INTERFACE(GTK_TYPE_EDITABLE, realPriceEntry_initEditable))

// the order we need to run our validation signals is:
// 	our insert-text -> GtkEntry insert-text -> our validation -> GtkEntry changed -> other connected changes
// so what we do is
// 	- override the vtable insert-text (nothing else connects to insert-text
// 	- connect to our own changed before anything else does
// thanks to ebassi, mclasen, and halfline in irc.gimp.net/#gtk+
// TODO rearrange eveyrthing to have related things close together

static void realPriceEntryValidate(RealPriceEntry *p)
{
	const gchar *amount;
	Price dollars = 0, cents = 0;
	gint i, n;

	amount = gtk_entry_get_text(GTK_ENTRY(p));
	if (amount == NULL)				// must enter something
		goto invalid;
	n = strlen(amount);
	if (n == 0)						// must enter something
		goto invalid;
	if (n == 1 && amount[0] == '.')		// just a . is invalid
		goto invalid;
	for (i = 0; i < n; i++) {
		if (amount[i] >= '0' && amount[i] <= '9') {
			dollars = dollars * 10 + (amount[i] - '0');
			continue;
		}
		if (amount[i] == '.')
			break;
		goto invalid;		// unrecognized character
	}
	if (i < n) {				// amount[i] == '.'
		i++;
		if (i == n)			// ###. is valid
			;
		else if (i != n - 2)	// must have either none or two digits
			goto invalid;
		else {
			cents = amount[i] - '0';
			cents = cents * 10 + (amount[i + 1] - '0');
		}
	}
	p->price = PRICE(dollars, cents);
	p->valid = TRUE;
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(p),
		GTK_ENTRY_ICON_PRIMARY, NULL);
	return;

invalid:
	p->valid = FALSE;
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(p),
		GTK_ENTRY_ICON_PRIMARY, "dialog-error");
}

static void realPriceEntry_changed(GtkEditable *editable, gpointer data)
{
	USED(editable);

	// validate before chaining up so the new validity shows up
	realPriceEntryValidate(REAL_PRICE_ENTRY(data));
}

static void realPriceEntry_init(RealPriceEntry *p)
{
	gtk_entry_set_width_chars(GTK_ENTRY(p), 10);		// not too long, not too short
	gtk_entry_set_alignment(GTK_ENTRY(p), 1);
	p->price = 0;
	p->valid = TRUE;
	g_signal_connect(p, "changed", G_CALLBACK(realPriceEntry_changed), p);
}

static void realPriceEntry_dispose(GObject *obj)
{
	G_OBJECT_CLASS(realPriceEntry_parent_class)->dispose(obj);
}

static void realPriceEntry_finalize(GObject *obj)
{
	G_OBJECT_CLASS(realPriceEntry_parent_class)->finalize(obj);
}

static GtkEditableInterface *chain = NULL;

static void realPriceEntry_insertText(GtkEditable *editable, const gchar *text, gint n, gint *pos)
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
	// let the text in
	if (chain->insert_text != NULL)
		chain->insert_text(editable, text, n, pos);
	// don't validate here; we need to validate between the chained ::insert-text and the chained ::change (otherwise things act weird)
}

static void realPriceEntry_initEditable(GtkEditableInterface *interface)
{
	chain = (GtkEditableInterface *) g_type_interface_peek_parent(interface);
#define CHAIN(x) interface->x = chain->x;
	// these came from gtk/gtkeditable.h because the docs don't list this interface??? TODO
	// signals
	interface->insert_text = realPriceEntry_insertText;
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

static void realPriceEntry_class_init(RealPriceEntryClass *class)
{
	G_OBJECT_CLASS(class)->dispose = realPriceEntry_dispose;
	G_OBJECT_CLASS(class)->finalize = realPriceEntry_finalize;
}

static GtkWidget *newRealPriceEntry(void)
{
	return (GtkWidget *) g_object_new(realPriceEntry_get_type(), NULL);
}

// PriceEntry contains a RealPriceEntry and the $ label.

struct PriceEntry {
	GtkGrid parent_instance;

	GtkWidget *label;
	GtkWidget *entry;
};

struct PriceEntryClass {
	GtkGridClass parent_class;
};

G_DEFINE_TYPE(PriceEntry, priceEntry, GTK_TYPE_GRID)

static void priceEntry_init(PriceEntry *p)
{
	p->label = gtk_label_new("$");
	gtk_grid_attach_next_to(GTK_GRID(p),
		p->label, NULL,
		GTK_POS_TOP, 1, 1);
	p->entry = newRealPriceEntry();
	gtk_widget_set_hexpand(p->entry, TRUE);
	gtk_widget_set_halign(p->entry, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(p),
		p->entry, p->label,
		GTK_POS_RIGHT, 1, 1);
}

static void priceEntry_dispose(GObject *obj)
{
	// DON'T UNREF p->label OR p->entry
	// the following call will do it for us
	// if we unref p->label or p->entry here ourselves, we unref a freed object
	G_OBJECT_CLASS(priceEntry_parent_class)->dispose(obj);
}

static void priceEntry_finalize(GObject *obj)
{
	G_OBJECT_CLASS(priceEntry_parent_class)->finalize(obj);
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

void priceEntryConnect(PriceEntry *p, char *signal, GCallback callback, gpointer data)
{
	g_signal_connect(p->entry, signal, callback, data);
}

gboolean priceEntryGetPrice(PriceEntry *pe, Price *pout)
{
	if (REAL_PRICE_ENTRY(pe->entry)->valid) {
		*pout = REAL_PRICE_ENTRY(pe->entry)->price;
		return TRUE;
	}
	return FALSE;
}

// TODO replace with SetPrice() and Clear() functions
const char *priceEntryText(PriceEntry *pe)
{
	return gtk_entry_get_text(GTK_ENTRY(pe->entry));
}

void priceEntrySetText(PriceEntry *pe, char *text)
{
	gtk_entry_set_text(GTK_ENTRY(pe->entry), text);
}

void priceEntrySetPrice(PriceEntry *p, Price price)
{
	char *pricestr;

	pricestr = priceToString(price, "");
	gtk_entry_set_text(GTK_ENTRY(p->entry), pricestr);
	g_free(pricestr);
	// the above set_text() also sets the appropriate fields
	// TODO this isn't really efficient, but it works... is there a safer way? :S
	// maybe I'll just add a validation check...
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
	// TODO right-align
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
