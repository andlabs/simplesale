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
extern void alignLabel(GtkWidget *, gfloat);
extern void expandWindowHeight(GtkWindow *, gint);
extern GtkWidget *newHeaderBar(char *, GtkWidget *);
extern GtkWidget *newConfirmHeaderButton(char *, GCallback, gpointer, GtkWidget *);
extern GtkWidget *newCancelHeaderButton(char *, GCallback, gpointer, GtkWidget *);
extern GtkWidget *newRegularHeaderButton(char *, GCallback, gpointer, GtkWidget *);
extern GtkWidget *newListScroller(GtkWidget *);
extern void attachLabel(char *, GtkWidget *, GtkWidget *);
extern gboolean askConfirm(GtkWidget *, char *, char *, ...);

// price.c
typedef guint64 Price;
#define PRICE(d, c) ((Price) ((((Price) (d)) * 100) + ((Price) (c))))
#define PRICEFMT G_GUINT64_FORMAT
#define PRICETYPE G_TYPE_UINT64
#define PRICEPARAMSPEC g_param_spec_uint64
#define PRICEVALUE g_value_get_uint64
#define PRICEMIN 0
#define PRICEMAX G_MAXUINT64
extern gchar *priceToString(Price, char *);
extern Price priceFromBytes(uint8_t[8]);
extern void priceToBytes(Price, uint8_t[8]);

#define PRICE_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), priceEntry_get_type(), PriceEntry))
#define PRICE_ENTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), priceEntry_get_type(), PriceEntryClass))
typedef struct PriceEntry PriceEntry;
typedef struct PriceEntryClass PriceEntryClass;
extern GtkWidget *newPriceEntry(void);
extern GType priceEntry_get_type(void);
extern void priceEntryConnect(PriceEntry *, char *, GCallback, gpointer);
extern gboolean priceEntryGetPrice(PriceEntry *, Price *);
extern const char *priceEntryText(PriceEntry *);
extern void priceEntrySetPrice(PriceEntry *, Price);
extern void priceEntryClear(PriceEntry *);

typedef struct PriceRenderer PriceRenderer;
extern GtkCellRenderer *newPriceRenderer();

// items.c
extern void initItems(void);
extern GtkTreeIter addItem(char *, Price);
extern void deleteItem(GtkTreeIter *);
extern void getItem(GtkTreeIter *, char **, Price *);
extern GtkTreeIter itemPathToIter(GtkTreePath *);
extern void setItemName(GtkTreeIter *, const char *);
extern void setItemPrice(GtkTreeIter *, Price);
extern GtkTreeModel *newItemsFiltered(void);
extern void setItemsIconLayout(GtkCellLayout *);
extern void setItemsModel(GtkTreeView *);
extern void setItemsColumnLayout(GtkTreeView *);
extern void loadItems(void);
extern void saveItems(void);

// itemeditor.c
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
extern const gchar *orderCustomer(Order *);
extern void orderShowWindow(Order *);
extern void orderHideWindow(Order *);

// shift.c
typedef struct Shift Shift;
extern Shift *newShift(char *);
extern void shiftShowWindow(Shift *);
extern void shiftHideWindow(Shift *);

// paydialog.c
typedef struct PayDialog PayDialog;
extern void freePayDialog(PayDialog *);
extern PayDialog *newPayDialog(GtkWindow *, Price);
extern gint runPayDialog(PayDialog *);
extern gboolean payDialogAmountPaid(PayDialog *, Price *);

// login.c
typedef struct Login Login;
extern Login *newLogin(void);

// accounts.c
extern void initAccounts(void);
extern void addAccount(char *, char *);
extern void setAccountsModelAndIconLayout(GtkIconView *);

typedef struct AccountEditor AccountEditor;
extern AccountEditor *newAccountEditor(void);

// passentry.c
#define PASS_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), passEntry_get_type(), PassEntry))
#define PASS_ENTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), passEntry_get_type(), PassEntryClass))
typedef struct PassEntry PassEntry;
typedef struct PassEntryClass PassEntryClass;
extern GtkWidget *newPassEntry(gboolean, GtkWidget *);
extern GType passEntry_get_type(void);
extern void resetPassEntry(PassEntry *);
extern const char *passEntryCurrentPassword(PassEntry *);
extern const char *passEntryNewPassword(PassEntry *);

// db.c
extern void initDB(void);
extern void endDB(void);
typedef struct dbIn dbIn;
extern dbIn *dbInOpenItems(void);
extern gboolean dbInReadItem(dbIn *, gchar **, Price *);
extern void dbInCommitAndFree(dbIn *);
typedef struct dbOut dbOut;
extern dbOut *dbOutOpenAndResetItems(void);
extern void dbOutWriteItemModel(GtkTreeModel *, dbOut *);
extern void dbOutCommitAndFree(dbOut *);
