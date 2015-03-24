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

// system.c
#define SystemType (System_get_type())
#define System(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SystemType, System))
#define IsSystem(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SystemType))
#define GetSystemInterface(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), SystemType, System))
typedef struct System System;
struct System {
	GTypeInterface parent_iface;

#define METHOD(n, ...) (*n)(System *, __VA_ARGS__)
#define METHOD0(n) (*n)(System *)
	// TODO shifts and orders

	void METHOD(SetItemsTreeView, GtkTreeView *);
	void METHOD(SetItemsIconView, GtkIconView *);
	void METHOD(AddItem, char *, Price);
	void METHOD(RemoveItem, GtkTreeIter *);
	char *METHOD(ItemName, GtkTreeIter *);
	Price METHOD(ItemPrice, GtkTreeIter *);
	void METHOD(SetItemName, GtkTreeIter *, char *);
	void METHOD(SetItemPrice, GtkTreeIter *, Price);

	void METHOD(SetEmployeesTreeView, GtkTreeView *);
	void METHOD(SetEmployeesIconView, GtkIconView *);
	void METHOD(AddEmployee, char *, char *);
	void METHOD(RemoveEmployee, GtkTreeIter *);
	char *METHOD(EmployeeName, GtkTreeIter *);
	gboolean METHOD(VerifyEmployeePassword, GtkTreeIter *, char *);
	void METHOD(SetEmployeeName, GtkTreeIter *, char *);
	void METHOD(ChangeEmployeePassword, GtkTreeIter *, char *, char *);

	// TODO devices

	// TODO tax
	gboolean METHOD(VerifyManagerPassword, char *);
	void METHOD(ChangeManagerPassword, char *, char *);

	void METHOD(AddMoney, Price);
	// TODO really gboolean?
	gboolean METHOD(RemoveMoney, Price);

	// TODO log?
	Price METHOD0(CurrentExpectedMoneyInDrawer);
#undef METHOD
#undef METHOD0
};
extern GType System_get_type(void);
extern System *system;
#define sysdo(n, ...) (*(system->n))(system, __VA_ARGS__)
#define sysdo0(n) (*(system->n))(system)

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
