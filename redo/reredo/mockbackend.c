// 27 march 2015
#include "simplesale.h"

struct MockBackendPriv {
	GtkListStore *items;
	GtkListStore *employees;
};

// TODO seriously?
static void MockBackend_Backend_init(BackendInterface *);

G_DEFINE_TYPE_WITH_CODE(MockBackend, MockBackend, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE(BackendType, MockBackend_Backend_init))

static void MockBackend_init(MockBackend *mb)
{
	mb->priv = G_TYPE_INSTANCE_GET_PRIVATE(mb, MockBackendType, struct MockBackendPriv);
	mb->priv->items = gtk_list_store_new(2,
		G_TYPE_STRING,		// name
		PriceType);			// price
	mb->priv->employees = gtk_list_store_new(2,
		G_TYPE_STRING,		// name
		G_TYPE_BYTE_ARRAY);	// password
}

static void MockBackend_dispose(GObject *obj)
{
	MockBackend *mb = MockBackend(obj);

	g_object_unref(mb->priv->items);
	mb->priv->items = NULL;
	g_object_unref(mb->priv->employees);
	mb->priv->employees = NULL;
	G_OBJECT_CLASS(MockBackend_parent_class)->dispose(obj);
}

static void MockBackend_finalize(GObject *obj)
{
	G_OBJECT_CLASS(MockBackend_parent_class)->finalize(obj);
}

static ShiftHandle MockBackendLogin(Backend *this, GtkTreeIter *employeeIter, char *password)
{
	// TODO
	return 0;
}

static void MockBackendClockOut(Backend *this, ShiftHandle shift)
{
	// TODO
}

static gboolean MockBackendStartManagerSession(Backend *this, char *password)
{
	// TODO
	return FALSE;
}

static void MockBackendStopManagerSession(Backend *this)
{
	// TODO
}

static void MockBackendFileOrder(Backend *this, ShiftHandle shift, Order *order, gboolean takeout)
{
	// TODO
}

static void MockBackendSetItemsTreeView(Backend *this, GtkTreeView *tv)
{
	// TODO
}

static void MockBackendSetItemsIconView(Backend *this, GtkIconView *iv)
{
	// TODO
}

static void MockBackendAppendItem(Backend *this, char *name, Price price)
{
	// TODO
}

static void MockBackendDeleteItem(Backend *this, GtkTreeIter *itemIter)
{
	// TODO
}

static char *MockBackendItemName(Backend *this, GtkTreeIter *itemIter)
{
	// TODO
	return NULL;
}

static Price MockBackendItemPrice(Backend *this, GtkTreeIter *itemIter)
{
	// TODO
	return 0;
}

static void MockBackendSetItemName(Backend *this, GtkTreeIter *itemIter, char *name)
{
	// TODO
}

static void MockBackendSetItemPrice(Backend *this, GtkTreeIter *itemIter, Price price)
{
	// TODO
}

static void MockBackendSetEmployeesTreeView(Backend *this, GtkTreeView *tv)
{
	// TODO
}

static void MockBackendSetEmployeesIconView(Backend *this, GtkIconView *iv)
{
	// TODO
}

static void MockBackendAppendEmployee(Backend *this, char *name, char *password)
{
	// TODO
}

static void MockBackendDeleteEmployee(Backend *this, GtkTreeIter *employeeIter)
{
	// TODO
}

static char *MockBackendEmployeeName(Backend *this, GtkTreeIter *employeeIter)
{
	// TODO
	return NULL;
}

static void MockBackendSetEmployeeName(Backend *this, GtkTreeIter *employeeIter, char *name)
{
	// TODO
}

static gboolean MockBackendChangeEmployeePassword(Backend *this, GtkTreeIter *employeeIter, char *oldPassword, char *newPassword)
{
	// TODO
	return FALSE;
}

static gdouble MockBackendTax(Backend *this)
{
	// TODO
	return 0;
}

static void MockBackendSetTax(Backend *this, gdouble tax)
{
	// TODO
}

static gboolean MockBackendChangeManagerPassword(Backend *this, char *oldPassword, char *newPassword)
{
	// TODO
	return FALSE;
}

static void MockBackendAddMoney(Backend *this, Price amount)
{
	// TODO
}

static gboolean MockBackendRemoveMoney(Backend *this, Price amount)
{
	// TODO
	return FALSE;
}

static Price MockBackendCurrentExpectedMoneyInDrawer(Backend *this)
{
	// TODO
	return 0;
}

static void MockBackendBeginReadLog(Backend *this)
{
	// TODO
}

static LogEntry *MockBackendNextLogEntry(Backend *this)
{
	// TODO
	return NULL;
}

static Price MockBackendEndReadLog(Backend *this)
{
	// TODO
	return 0;
}

static void MockBackend_Backend_init(BackendInterface *iface)
{
	iface->Login = MockBackendLogin;
	iface->ClockOut = MockBackendClockOut;
	iface->StartManagerSession = MockBackendStartManagerSession;
	iface->StopManagerSession = MockBackendStopManagerSession;
	iface->FileOrder = MockBackendFileOrder;
	iface->SetItemsTreeView = MockBackendSetItemsTreeView;
	iface->SetItemsIconView = MockBackendSetItemsIconView;
	iface->AppendItem = MockBackendAppendItem;
	iface->DeleteItem = MockBackendDeleteItem;
	iface->ItemName = MockBackendItemName;
	iface->ItemPrice = MockBackendItemPrice;
	iface->SetItemName = MockBackendSetItemName;
	iface->SetItemPrice = MockBackendSetItemPrice;
	iface->SetEmployeesTreeView = MockBackendSetEmployeesTreeView;
	iface->SetEmployeesIconView = MockBackendSetEmployeesIconView;
	iface->AppendEmployee = MockBackendAppendEmployee;
	iface->DeleteEmployee = MockBackendDeleteEmployee;
	iface->EmployeeName = MockBackendEmployeeName;
	iface->SetEmployeeName = MockBackendSetEmployeeName;
	iface->ChangeEmployeePassword = MockBackendChangeEmployeePassword;
	iface->Tax = MockBackendTax;
	iface->SetTax = MockBackendSetTax;
	iface->ChangeManagerPassword = MockBackendChangeManagerPassword;
	iface->AddMoney = MockBackendAddMoney;
	iface->RemoveMoney = MockBackendRemoveMoney;
	iface->CurrentExpectedMoneyInDrawer = MockBackendCurrentExpectedMoneyInDrawer;
	iface->BeginReadLog = MockBackendBeginReadLog;
	iface->NextLogEntry = MockBackendNextLogEntry;
	iface->EndReadLog = MockBackendEndReadLog;
	verifyBackendImpl("MockBackend", iface);
}

static void MockBackend_class_init(MockBackendClass *class)
{
	g_type_class_add_private(class, sizeof (struct MockBackendPriv));

	G_OBJECT_CLASS(class)->dispose = MockBackend_dispose;
	G_OBJECT_CLASS(class)->finalize = MockBackend_finalize;
}
