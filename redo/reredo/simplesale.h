// 23 march 2015
// TODO version macros
#include <gtk/gtk.h>
#include <stdint.h>

enum {
	gtkMargin = 12,
	gtkXPadding = 12,
	gtkYPadding = 6,
};

// util.c
extern void setName0Price1TreeViewColumnLayout(GtkTreeView *);

// price.c
typedef guint64 Price;
#define PriceType G_TYPE_UINT64
#define PriceParamSpec g_param_spec_uint64
// TODO rename these
#define g_value_get_Price(v) g_value_get_uint64(v)
#define g_value_set_Price(v, p) g_value_set_uint64(v, p)
#define PriceFormat G_GUINT64_FORMAT
#define PriceMin 0
#define PriceMax G_MAXUINT64
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

// pricerenderer.c
#define PriceRendererType (PriceRenderer_get_type())
#define PriceRenderer(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PriceRendererType, PriceRenderer))
#define IsPriceRenderer(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PriceRendererType))
#define PriceRendererClass(class) (G_TYPE_CHECK_CLASS_CAST((class), PriceRendererType, PriceRendererClass))
#define IsPriceRendererClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), PriceRenderer))
#define GetPriceRendererClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), PriceRendererType, PriceRendererClass))
typedef struct PriceRenderer PriceRenderer;
typedef struct PriceRendererClass PriceRendererClass;
struct PriceRenderer {
	GtkCellRendererText parent_instance;
	struct PriceRendererPriv *priv;
};
struct PriceRendererClass {
	GtkCellRendererTextClass parent_class;
};
extern GType PriceRenderer_get_type(void);
extern GtkCellRenderer *newPriceRenderer(void);

// TODO sort this out
typedef gint64 ShiftHandle;		// good for g_get_real_time()
typedef struct Order Order;
typedef struct LogEntry LogEntry;

// zbackend.c
#include "zbackend.h"
extern Backend *backend;

// mockbackend.c
#define MockBackendType (MockBackend_get_type())
#define MockBackend(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), MockBackendType, MockBackend))
#define IsMockBackend(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MockBackendType))
#define MockBackendClass(class) (G_TYPE_CHECK_CLASS_CAST((class), MockBackendType, MockBackendClass))
#define IsMockBackendClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), MockBackend))
#define GetMockBackendClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MockBackendType, MockBackendClass))
typedef struct MockBackend MockBackend;
typedef struct MockBackendClass MockBackendClass;
struct MockBackend {
	GObject parent_instance;
	struct MockBackendPriv *priv;
};
struct MockBackendClass {
	GObjectClass parent_class;
};
extern GType MockBackend_get_type(void);

// manager.c
extern gboolean manager(void);

// itemeditor.c
extern GtkWidget *newItemEditor(void);

// employeeeditor.c
extern GtkWidget *newEmployeeEditor(void);

// deviceeditor.c
extern GtkWidget *newDeviceEditor(void);

// othersettings.c
extern GtkWidget *newOtherSettings(void);

// addremovemoney.c
extern GtkWidget *newAddRemoveMoney(void);

// logviewer.c
extern GtkWidget *newLogViewer(void);
