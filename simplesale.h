// 14 june 2014
#define GLIB_VERSION_MIN_REQUIRED GLIB_VERSION_2_40
#define GLIB_VERSION_MAX_ALLOWED GLIB_VERSION_2_40
#define GDK_VERSION_MIN_REQUIRED GDK_VERSION_3_10
#define GDK_VERSION_MAX_ALLOWED GDK_VERSION_3_10
#include <gtk/gtk.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define USED(...) ((void) (__VA_ARGS__))

// util.c
typedef guint64 price;
#define PRICE(d, c) ((price) ((((price) (d)) * 100) + ((price) (c))))
#define PRICEFMT G_GUINT64_FORMAT
extern gint listStoreIterIndex(GtkTreeModel *, GtkTreeIter *);
extern gint listStorePathIndex(GtkTreePath *);
extern gchar *priceToString(price, char *);

// orderwin.c
typedef struct orderWindow orderWindow;
extern orderWindow *newOrderWindow(void);

// items.c
extern void initItems(void);
extern void addItem(char *, price);
extern void getItem(gint, char **, char **, price *);
extern GtkListStore *itemsStore(void);
extern GtkTreeModel *itemsModel(void);
extern void setItemsIconLayout(GtkCellLayout *);

// order.c
typedef struct order order;
extern order *newOrder(void);
extern void addToOrder(order *, gint);
extern void removeFromOrder(order *, GtkTreeIter *);
extern price subtotal(order *);
extern GtkTreeModel *orderModel(order *);
extern void setOrderTableLayout(GtkTreeView *);
