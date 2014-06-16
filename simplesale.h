// 14 june 2014
#define GLIB_VERSION_MIN_REQUIRED GLIB_VERSION_2_40
#define GLIB_VERSION_MAX_ALLOWED GLIB_VERSION_2_40
#define GDK_VERSION_MIN_REQUIRED GDK_VERSION_3_10
#define GDK_VERSION_MAX_ALLOWED GDK_VERSION_3_10
#include <gtk/gtk.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define USED(x) ((void) (x))

// util.c
extern gint listStoreIterIndex(GtkTreeModel *, GtkTreeIter *);
extern gint listStorePathIndex(GtkTreePath *);
extern void alignLabel(GtkWidget *, gfloat);

// price.c
typedef guint64 Price;
#define PRICE(d, c) ((Price) ((((Price) (d)) * 100) + ((Price) (c))))
#define PRICEFMT G_GUINT64_FORMAT
#define PRICETYPE G_TYPE_UINT64
extern gchar *priceToString(Price, char *);

#define PRICE_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), priceEntry_get_type(), PriceEntry))
#define PRICE_ENTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), priceEntry_get_type(), PriceEntryClass))
typedef struct PriceEntry PriceEntry;
typedef struct PriceEntryClass PriceEntryClass;
struct PriceEntry {
	GtkEntry parent_instance;
};
struct PriceEntryClass {
	GtkEntryClass parent_class;
};
extern GtkWidget *newPriceEntry(void);
extern GType priceEntry_get_type(void);
enum {
	priceEntryOK,
	priceEntryEmpty,
	priceEntryInvalid,
};
extern int priceEntryGetPrice(PriceEntry *, Price *);

// items.c
extern void initItems(void);
extern void addItem(char *, Price);
extern void getItem(gint, char **, char **, Price *);
extern GtkListStore *itemsStore(void);
extern GtkTreeModel *itemsModel(void);
extern void setItemsIconLayout(GtkCellLayout *);
extern void setItemsColumnLayout(GtkTreeView *);

typedef struct ItemEditor ItemEditor;
extern ItemEditor *newItemEditor(void);

// order.c
enum {
	orderCancel,
	orderPayNow,
	orderPayLater,
};
typedef struct Order Order;
extern Order *newOrder(void);
extern void freeOrder(Order *);
extern void addToOrder(Order *, gint);
extern void removeFromOrder(Order *, GtkTreeIter *);
extern Price subtotal(Order *);
extern GtkTreeModel *orderModel(Order *);
extern void setOrderTableLayout(GtkTreeView *);
extern const gchar *orderCustomer(Order *);
extern void orderShowWindow(Order *);
extern void orderHideWindow(Order *);

// shift.c
typedef struct Shift Shift;
extern Shift *newShift(char *);
extern void shiftNewOrder(Shift *);
extern void shiftShowWindow(Shift *);
extern void shiftHideWindow(Shift *);

// paydialog.c
typedef struct PayDialog PayDialog;
extern void freePayDialog(PayDialog *);
extern PayDialog *newPayDialog(GtkWindow *, Price);
extern gint runPayDialog(PayDialog *);
extern gboolean payDialogAmountPaid(PayDialog *, Price *);

// login.c
extern void initAccounts(void);
extern void addAccount(char *, char *);

typedef struct Login Login;
extern Login *newLogin(void);
