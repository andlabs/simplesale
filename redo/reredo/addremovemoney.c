// 24 march 2015
#include "simplesale.h"

#define AddRemoveMoneyType (AddRemoveMoney_get_type())
#define AddRemoveMoney(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), AddRemoveMoneyType, AddRemoveMoney))
#define IsAddRemoveMoney(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), AddRemoveMoneyType))
#define AddRemoveMoneyClass(class) (G_TYPE_CHECK_CLASS_CAST((class), AddRemoveMoneyType, AddRemoveMoneyClass))
#define IsAddRemoveMoneyClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), AddRemoveMoney))
#define GetAddRemoveMoneyClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), AddRemoveMoneyType, AddRemoveMoneyClass))
typedef struct AddRemoveMoney AddRemoveMoney;
typedef struct AddRemoveMoneyClass AddRemoveMoneyClass;
struct AddRemoveMoney {
	GtkGrid parent_instance;
	struct AddRemoveMoneyPrivate *priv;
};
struct AddRemoveMoneyClass {
	GtkGridClass parent_class;
};
static GType AddRemoveMoney_get_type(void);

typedef struct AddRemoveMoneyPrivate AddRemoveMoneyPrivate;

struct AddRemoveMoneyPrivate {
	GtkWidget *current;
	GtkWidget *changeAmount;
	GtkWidget *change;
	GtkWidget *addAmount;
	GtkWidget *removeAmount;
};

G_DEFINE_TYPE_WITH_CODE(AddRemoveMoney, AddRemoveMoney, GTK_TYPE_GRID,
	G_ADD_PRIVATE(AddRemoveMoney))

static void AddRemoveMoney_init(AddRemoveMoney *a)
{
	a->priv = AddRemoveMoney_get_instance_private(a);

	gtk_widget_init_template(GTK_WIDGET(a));

	// Glade doesn't let me set these two
	gtk_button_set_image(GTK_BUTTON(a->priv->addAmount), gtk_image_new_from_icon_name("list-add-symbolic", GTK_ICON_SIZE_MENU));
	gtk_button_set_image(GTK_BUTTON(a->priv->removeAmount), gtk_image_new_from_icon_name("list-remove-symbolic", GTK_ICON_SIZE_MENU));
}

static void AddRemoveMoney_dispose(GObject *obj)
{
	G_OBJECT_CLASS(AddRemoveMoney_parent_class)->dispose(obj);
}

static void AddRemoveMoney_finalize(GObject *obj)
{
	G_OBJECT_CLASS(AddRemoveMoney_parent_class)->finalize(obj);
}

static void AddRemoveMoney_class_init(AddRemoveMoneyClass *class)
{
	G_OBJECT_CLASS(class)->dispose = AddRemoveMoney_dispose;
	G_OBJECT_CLASS(class)->finalize = AddRemoveMoney_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/addremovemoney.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), AddRemoveMoney, name)
	T(current);
	T(changeAmount);
	T(change);
	T(addAmount);
	T(removeAmount);
#undef T
}

GtkWidget *newAddRemoveMoney(void)
{
	return GTK_WIDGET(g_object_new(AddRemoveMoneyType, NULL));
}
