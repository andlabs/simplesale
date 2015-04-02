// 1 april 2015
#include "simplesale.h"

typedef struct PayDialogPrivate PayDialogPrivate;

struct PayDialogPrivate {
};

G_DEFINE_TYPE_WITH_CODE(PayDialog, PayDialog, GTK_TYPE_DIALOG,
	G_ADD_PRIVATE(PayDialog))

static void PayDialog_init(PayDialog *p)
{
	p->priv = PayDialog_get_instance_private(p);

	gtk_widget_init_template(GTK_WIDGET(p));
}

static void PayDialog_dispose(GObject *obj)
{
	G_OBJECT_CLASS(PayDialog_parent_class)->dispose(obj);
}

static void PayDialog_finalize(GObject *obj)
{
	G_OBJECT_CLASS(PayDialog_parent_class)->finalize(obj);
}

static void PayDialog_class_init(PayDialogClass *class)
{
	G_OBJECT_CLASS(class)->dispose = PayDialog_dispose;
	G_OBJECT_CLASS(class)->finalize = PayDialog_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/paydialog.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), PayDialog, name)
//TODO	T(tax);
//TODO	T(changeManagerPassword);
#undef T
}

// TODO parent window? dialog flags?
GtkWidget *newPayDialog(void)
{
	return GTK_WIDGET(g_object_new(PayDialogType, NULL));
}
