// 23 march 2015
// TODO version macros
#include <gtk/gtk.h>
#include <stdint.h>

enum {
	gtkMargin = 12,
	gtkXPadding = 12,
	gtkYPadding = 6,
};

// price.c
typedef guint64 Price;
#define PriceType G_TYPE_UINT64
#define g_value_get_Price(v) g_value_get_uint64(v)
#define g_value_set_Price(v, p) g_value_set_uint64(v, p)
#define PriceFormat G_GUINT64_FORMAT
extern char *priceString(Price);

// priceentry.c
#define PriceEntryType (PriceEntry_get_type())
#define PriceEntry(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PriceEntryType, PriceEntry))
#define IsPriceEntry(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PriceEntryType))
#define PriceEntryClass(class) (G_TYPE_CHECK_CLASS_CAST((class), PriceEntryType, PriceEntryClass))
#define IsPriceEntryClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), PriceEntry))
#define GetPriceEntryClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), PriceEntryType, PriceEntryClass))
typedef struct PriceEntry PriceEntry;
typedef struct PriceEntryClass PriceEntryClass;
struct PriceEntry {
	GtkEntry parent_instance;
	struct PriceEntryPriv *priv;
};
struct PriceEntryClass {
	GtkEntryClass parent_class;
};
extern GType PriceEntry_get_type(void);

// manager.c
extern gboolean manager(void);

// itemeditor.c
extern GtkWidget *newItemEditor(void);

// employeeeditor.c
extern GtkWidget *newEmployeeEditor(void);

// othersettings.c
extern GtkWidget *newOtherSettings(void);

// addremovemoney.c
extern GtkWidget *newAddRemoveMoney(void);

// logviewer.c
extern GtkWidget *newLogViewer(void);
