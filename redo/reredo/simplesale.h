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

// order.c
typedef struct Order Order;
extern Order *newOrder(void);
extern void deleteOrder(Order *);
extern char *OrderCustomerName(Order *);
extern void OrderSetCustomerName(Order *, char *);
extern void OrderSetTreeView(Order *, GtkTreeView *);
extern void OrderAppendItem(Order *, GtkTreeIter *);
extern void OrderAppendNote(Order *, char *);
extern void OrderDeleteItem(Order *, GtkTreeIter *);
extern Price OrderTotal(Order *);
extern Price OrderSubtotal(Order *);

// TODO sort this out
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

// orderwindow.c
#define OrderWindowType (OrderWindow_get_type())
#define OrderWindow(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), OrderWindowType, OrderWindow))
#define IsOrderWindow(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), OrderWindowType))
#define OrderWindowClass(class) (G_TYPE_CHECK_CLASS_CAST((class), OrderWindowType, OrderWindowClass))
#define IsOrderWindowClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), OrderWindow))
#define GetOrderWindowClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), OrderWindowType, OrderWindowClass))
typedef struct OrderWindow OrderWindow;
typedef struct OrderWindowClass OrderWindowClass;
struct OrderWindow {
	GtkWindow parent_instance;
	struct OrderWindowPrivate *priv;
};
struct OrderWindowClass {
	GtkWindowClass parent_class;
};
extern GType OrderWindow_get_type(void);
extern OrderWindow *newOrderWindow(void);

// paydialog.c
#define PayDialogType (PayDialog_get_type())
#define PayDialog(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PayDialogType, PayDialog))
#define IsPayDialog(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PayDialogType))
#define PayDialogClass(class) (G_TYPE_CHECK_CLASS_CAST((class), PayDialogType, PayDialogClass))
#define IsPayDialogClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), PayDialog))
#define GetPayDialogClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), PayDialogType, PayDialogClass))
typedef struct PayDialog PayDialog;
typedef struct PayDialogClass PayDialogClass;
struct PayDialog {
	GtkDialog parent_instance;
	struct PayDialogPrivate *priv;
};
struct PayDialogClass {
	GtkDialogClass parent_class;
};
extern GType PayDialog_get_type(void);
extern GtkWidget *newPayDialog(void);
enum {
	PayDialogResponsePayNow = 1,
	PayDialogResponsePayLater,
};

// manager.c
#define ManagerType (Manager_get_type())
#define Manager(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ManagerType, Manager))
#define IsManager(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ManagerType))
#define ManagerClass(class) (G_TYPE_CHECK_CLASS_CAST((class), ManagerType, ManagerClass))
#define IsManagerClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), Manager))
#define GetManagerClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), ManagerType, ManagerClass))
typedef struct Manager Manager;
typedef struct ManagerClass ManagerClass;
struct Manager {
	GtkWindow parent_instance;
	struct ManagerPrivate *priv;
};
struct ManagerClass {
	GtkWindowClass parent_class;
};
extern GType Manager_get_type(void);
extern Manager *newManager(void);

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
