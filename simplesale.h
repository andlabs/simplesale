/* 14 june 2014 */
#define GLIB_VERSION_MIN_REQUIRED GLIB_VERSION_2_40
#define GLIB_VERSION_MAX_ALLOWED GLIB_VERSION_2_40
#define GDK_VERSION_MIN_REQUIRED GDK_VERSION_3_10
#define GDK_VERSION_MAX_ALLOWED GDK_VERSION_3_10
#include <gtk/gtk.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* orderwin.c */
typedef struct orderWindow orderWindow;
extern orderWindow *newOrderWindow(void);

/* items.c */
extern void initItems(void);
extern void addItem(char *, char *);
extern GtkListStore *itemsStore(void);
extern GtkTreeModel *itemsModel(void);
extern void setItemsIconLayout(GtkCellLayout *);

/* order.c */
typedef struct order order;
extern order *newOrder(void);
extern void addToOrder(order *, gint);
extern void removeFromOrder(order *, GtkTreeIter *);
extern GtkTreeModel *orderModel(order *);
extern void setOrderTableLayout(GtkTreeView *);

/* util.c */
extern gint listStoreIterIndex(GtkTreeModel *, GtkTreeIter *);
extern gint listStorePathIndex(GtkTreeModel *, GtkTreePath *);
