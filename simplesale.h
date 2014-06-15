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
typedef guint64 price;
#define PRICE(d, c) ((price) ((((price) (d)) * 100) + ((price) (c))))
#define PRICEFMT G_GUINT64_FORMAT
extern gint listStoreIterIndex(GtkTreeModel *, GtkTreeIter *);
extern gint listStorePathIndex(GtkTreePath *);
extern gchar *priceToString(price, char *);
extern void alignLabel(GtkWidget *, gfloat);

// items.c
extern void initItems(void);
extern void addItem(char *, price);
extern void getItem(gint, char **, char **, price *);
extern GtkListStore *itemsStore(void);
extern GtkTreeModel *itemsModel(void);
extern void setItemsIconLayout(GtkCellLayout *);

// order.c
typedef struct shift shift;	// TODO make this unnecessary
typedef struct Order Order;
extern Order *newOrder(shift *);
extern void freeOrder(Order *);
extern void addToOrder(Order *, gint);
extern void removeFromOrder(Order *, GtkTreeIter *);
extern price subtotal(Order *);
extern GtkTreeModel *orderModel(Order *);
extern void setOrderTableLayout(GtkTreeView *);
extern const gchar *orderCustomer(Order *);
extern void orderShowWindow(Order *);
extern void orderHideWindow(Order *);

// shift.c
enum {
	orderCancel,
	orderPayNow,
	orderPayLater,
};
//typedef struct shift shift;
extern shift *newShift(char *);
extern void shiftNewOrder(shift *);
extern void shiftDoOrder(shift *, Order *, int);

// paydialog.c
typedef struct payDialog payDialog;
extern void freePayDialog(payDialog *);
extern payDialog *newPayDialog(GtkWindow *, Order *);
extern gint runPayDialog(payDialog *);
extern gboolean payDialogAmountPaid(payDialog *, price *);
