// 1 april 2015
#include "simplesale.h"

typedef struct PayDialogPrivate PayDialogPrivate;

struct PayDialogPrivate {
	GtkWidget *cost;
	GtkWidget *paid;
	GtkWidget *b[10];
	GtkWidget *bPoint;
	GtkWidget *bClear;
	GtkWidget *payCash;
	GtkWidget *payCredit;
	GtkWidget *cancel;
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
	int i;
	char s[3] = { 'b', '#', '\0' };
	gssize off;

	G_OBJECT_CLASS(class)->dispose = PayDialog_dispose;
	G_OBJECT_CLASS(class)->finalize = PayDialog_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/paydialog.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), PayDialog, name)
	T(cost);
	T(paid);
	// TODO verify that this is safe
	off = G_PRIVATE_OFFSET(PayDialog, b);
	for (i = 0; i < 10; i++) {
		s[1] = i + '0';
		gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(class), s, FALSE, off);
		off += sizeof (GtkWidget *);
	}
	GtkWidget *b[10];
	T(bPoint);
	T(bClear);
	T(payCash);
	T(payCredit);
	T(cancel);
#undef T
}

// TODO parent window? dialog flags?
GtkWidget *newPayDialog(void)
{
	return GTK_WIDGET(g_object_new(PayDialogType, NULL));
}
