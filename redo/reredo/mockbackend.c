// 27 march 2015
#include "simplesale.h"

struct MockBackendPriv {
	GtkListStore *items;
	GtkListStore *employees;
	GByteArray *manager;
};

// TODO seriously?
static void MockBackend_Backend_init(BackendInterface *);

G_DEFINE_TYPE_WITH_CODE(MockBackend, MockBackend, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE(BackendType, MockBackend_Backend_init))

static struct {
	char *name;
	Price price;
} mockItems[] = {
	{ "Cheese Pizza", 1000 },
	{ "Cheese Slice", 200 },
	{ "Soda", 125 },
	{ "Cookie", 100 },
	{ NULL, 0 },
};

static struct {
	char *name;
	char *pw;
} mockEmployees[] = {
	{ "Name 1", "" },
	{ "Name 2", "password" },
	{ "Name 3", "welp" },
	{ NULL, NULL },
};

#define mockManagerPassword ""

static GByteArray *staticStrToGByteArray(char *str)
{
	guint8 *m;

	m = (guint8 *) g_strdup(str);
	return g_byte_array_new_take(m, g_utf8_strlen(str, -1));
}

static void MockBackend_init(MockBackend *mb)
{
	GtkTreeIter iter;
	int i;

	mb->priv = G_TYPE_INSTANCE_GET_PRIVATE(mb, MockBackendType, struct MockBackendPriv);

	mb->priv->items = gtk_list_store_new(2,
		G_TYPE_STRING,		// name
		PriceType);			// price
	for (i = 0; mockItems[i].name != NULL; i++) {
		gtk_list_store_append(mb->priv->items, &iter);
		gtk_list_store_set(mb->priv->items, &iter,
			0, mockItems[i].name,
			1, mockItems[i].price,
			-1);
	}

	mb->priv->employees = gtk_list_store_new(2,
		G_TYPE_STRING,		// name
		G_TYPE_BYTE_ARRAY);	// password
	for (i = 0; mockEmployees[i].name != NULL; i++) {
		gtk_list_store_append(mb->priv->employees, &iter);
		gtk_list_store_set(mb->priv->employees, &iter,
			0, mockEmployees[i].name,
			1, staticStrToGByteArray(mockEmployees[i].pw),
			-1);
	}

	mb->priv->manager = staticStrToGByteArray(mockManagerPassword);
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

static gboolean pwsame(char *password, GByteArray *against)
{
	return memcmp(password, against->data, g_utf8_strlen(password, -1)) != 0;
}

static ShiftHandle MockBackendLogin(Backend *this, GtkTreeIter *employeeIter, char *password)
{
	MockBackend *mb = MockBackend(this);
	GByteArray *against;

	gtk_tree_model_get(GTK_TREE_MODEL(mb->priv->employees), employeeIter, 1, &against, -1);
	if (pwsame(password, against))
		return 0;
	// TODO
	return 0;
}

static void MockBackendClockOut(Backend *this, ShiftHandle shift)
{
	// TODO
}

static gboolean MockBackendStartManagerSession(Backend *this, char *password)
{
	MockBackend *mb = MockBackend(this);

	return pwsame(password, mb->priv->manager);
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
	// TODO split into separate functions
	MockBackend *mb = MockBackend(this);
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;

	gtk_tree_view_set_model(tv, GTK_TREE_MODEL(mb->priv->items));
	// TODO clear columns
	r = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("Item", r, "text", 0, NULL);
	gtk_tree_view_column_set_expand(col, TRUE);
	gtk_tree_view_append_column(tv, col);
	r = newPriceRenderer();
	col = gtk_tree_view_column_new_with_attributes("Price", r, "price", 1, NULL);
	gtk_tree_view_append_column(tv, col);
	gtk_tree_view_set_headers_visible(tv, TRUE);
}

static void MockBackendSetItemsIconView(Backend *this, GtkIconView *iv)
{
	// TODO split into separate functions
	MockBackend *mb = MockBackend(this);
	GtkCellLayout *layout;
	GtkCellRenderer *r;

	gtk_icon_view_set_model(iv, GTK_TREE_MODEL(mb->priv->items));
	layout = GTK_CELL_LAYOUT(iv);		// TODO inline?
	gtk_cell_layout_clear(layout);
	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(layout, r, TRUE);
	gtk_cell_layout_set_attributes(layout, r, "text", 0, NULL);
	r = newPriceRenderer();
	gtk_cell_layout_pack_start(layout, r, FALSE);
	gtk_cell_layout_set_attributes(layout, r, "price", 1, NULL);
}

static void MockBackendAppendItem(Backend *this, char *name, Price price)
{
	MockBackend *mb = MockBackend(this);
	GtkTreeIter iter;

	gtk_list_store_append(mb->priv->items, &iter);
	gtk_list_store_set(mb->priv->items, &iter, 0, name, 1, price, -1);
}

static void MockBackendDeleteItem(Backend *this, GtkTreeIter *itemIter)
{
	MockBackend *mb = MockBackend(this);

	gtk_list_store_remove(mb->priv->items, itemIter);
}

static char *MockBackendItemName(Backend *this, GtkTreeIter *itemIter)
{
	MockBackend *mb = MockBackend(this);
	char *name;

	gtk_tree_model_get(GTK_TREE_MODEL(mb->priv->items), itemIter, 0, &name, -1);
	// TODO really?
	return g_strdup(name);
}

static Price MockBackendItemPrice(Backend *this, GtkTreeIter *itemIter)
{
	MockBackend *mb = MockBackend(this);
	Price price;

	gtk_tree_model_get(GTK_TREE_MODEL(mb->priv->items), itemIter, 1, &price, -1);
	return price;
}

static void MockBackendSetItemName(Backend *this, GtkTreeIter *itemIter, char *name)
{
	MockBackend *mb = MockBackend(this);

	gtk_list_store_set(mb->priv->items, itemIter, 0, name, -1);
}

static void MockBackendSetItemPrice(Backend *this, GtkTreeIter *itemIter, Price price)
{
	MockBackend *mb = MockBackend(this);

	gtk_list_store_set(mb->priv->items, itemIter, 1, price, -1);
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
