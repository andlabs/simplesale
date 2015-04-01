// 24 march 2015
#include "simplesale.h"

// TODO
// - make sure the problem with the item editor removal doesn't happen here

#define EmployeeEditorType (EmployeeEditor_get_type())
#define EmployeeEditor(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EmployeeEditorType, EmployeeEditor))
#define IsEmployeeEditor(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EmployeeEditorType))
#define EmployeeEditorClass(class) (G_TYPE_CHECK_CLASS_CAST((class), EmployeeEditorType, EmployeeEditorClass))
#define IsEmployeeEditorClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), EmployeeEditor))
#define GetEmployeeEditorClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), EmployeeEditorType, EmployeeEditorClass))
typedef struct EmployeeEditor EmployeeEditor;
typedef struct EmployeeEditorClass EmployeeEditorClass;
struct EmployeeEditor {
	GtkBox parent_instance;
	struct EmployeeEditorPrivate *priv;
};
struct EmployeeEditorClass {
	GtkBoxClass parent_class;
};
static GType EmployeeEditor_get_type(void);

typedef struct EmployeeEditorPrivate EmployeeEditorPrivate;

struct EmployeeEditorPrivate {
	gboolean selected;
	GtkTreeIter selectedIter;		// TODO rename selection?
	gulong nameChangedHandler;
	GtkWidget *list;
	GtkTreeSelection *listSelection;
	GtkWidget *addButton;
	GtkWidget *removeButton;
	GtkWidget *search;
	GtkWidget *name;
	GtkWidget *changePassword;
};

G_DEFINE_TYPE_WITH_CODE(EmployeeEditor, EmployeeEditor, GTK_TYPE_BOX,
	G_ADD_PRIVATE(EmployeeEditor))


static void nameChanged(GtkEditable *editable, gpointer data)
{
	EmployeeEditor *e = (EmployeeEditor *) data;

	if (!e->priv->selected)
		g_error("employee name changed with no employee selected");
	BackendSetEmployeeName(backend, &(e->priv->selectedIter), gtk_entry_get_text(GTK_ENTRY(e->priv->name)));
}

static void changePassword(GtkEditable *editable, gpointer data)
{
	EmployeeEditor *e = (EmployeeEditor *) data;

	if (!e->priv->selected)
		g_error("employee password change requested with no employee selected");
	// TODO
}

static void selectionChanged(GtkTreeSelection *sel, gpointer data)
{
	EmployeeEditor *e = EmployeeEditor(data);

	e->priv->selected = gtk_tree_selection_get_selected(e->priv->listSelection, NULL, &(e->priv->selectedIter));
	gtk_widget_set_sensitive(e->priv->name, e->priv->selected);
	gtk_widget_set_sensitive(e->priv->changePassword, e->priv->selected);
	gtk_widget_set_sensitive(e->priv->removeButton, e->priv->selected);
	if (e->priv->selected)
		gtk_entry_set_text(GTK_ENTRY(e->priv->name), BackendEmployeeName(backend, &(e->priv->selectedIter)));
	else {
		// clear the fields to make deselection make sense
		// we need to disable our signal handlers to prevent the g_error() calls in our signal handlers from firing
		// TODO really necessary?
		g_signal_handler_block(e->priv->name, e->priv->nameChangedHandler);
		gtk_entry_set_text(GTK_ENTRY(e->priv->name), "");
		g_signal_handler_unblock(e->priv->name, e->priv->nameChangedHandler);
	}
}

static void addEmployee(GtkToolButton *b, gpointer data)
{
	// TODO
}

static void removeEmployee(GtkToolButton *b, gpointer data)
{
	EmployeeEditor *e = (EmployeeEditor *) data;

	if (!e->priv->selected)
		g_error("remove employee button clicked without any employee selected");
	BackendDeleteEmployee(backend, &(e->priv->selectedIter));
}

static void EmployeeEditor_init(EmployeeEditor *e)
{
	e->priv = EmployeeEditor_get_instance_private(e);

	gtk_widget_init_template(GTK_WIDGET(e));

	BackendSetEmployeesTreeView(backend, GTK_TREE_VIEW(e->priv->list));
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(e->priv->list), GTK_ENTRY(e->priv->search));
	// TODO search should do case-insensitive substring or word-string match

	e->priv->nameChangedHandler = g_signal_connect(e->priv->name, "changed", G_CALLBACK(nameChanged), e);
	g_signal_connect(e->priv->changePassword, "clicked", G_CALLBACK(changePassword), e);
	g_signal_connect(e->priv->listSelection, "changed", G_CALLBACK(selectionChanged), e);
	g_signal_connect(e->priv->addButton, "clicked", G_CALLBACK(addEmployee), e);
	g_signal_connect(e->priv->removeButton, "clicked", G_CALLBACK(removeEmployee), e);

	// get the ball rolling
	// TODO make this unnecessary
	selectionChanged(e->priv->listSelection, e);
}

static void EmployeeEditor_dispose(GObject *obj)
{
	G_OBJECT_CLASS(EmployeeEditor_parent_class)->dispose(obj);
}

static void EmployeeEditor_finalize(GObject *obj)
{
	G_OBJECT_CLASS(EmployeeEditor_parent_class)->finalize(obj);
}

static void EmployeeEditor_class_init(EmployeeEditorClass *class)
{
	G_OBJECT_CLASS(class)->dispose = EmployeeEditor_dispose;
	G_OBJECT_CLASS(class)->finalize = EmployeeEditor_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/employeeeditor.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), EmployeeEditor, name)
	T(list);
	T(listSelection);
	T(addButton);
	T(removeButton);
	T(search);
	T(name);
	T(changePassword);
#undef T
}

GtkWidget *newEmployeeEditor(void)
{
	return GTK_WIDGET(g_object_new(EmployeeEditorType, NULL));
}
