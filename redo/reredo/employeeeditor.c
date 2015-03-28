// 24 march 2015
#include "simplesale.h"

struct EmployeeEditorPriv {
	gboolean selected;
	GtkTreeIter selectedIter;		// TODO rename selection?
	gulong nameChangedHandler;
};

#include "zemployeeeditor.h"

static void destroy(GtkWidget *widget, gpointer data)
{
	EmployeeEditor *e = (EmployeeEditor *) data;

	g_free(e);
}

static void nameChanged(GtkEditable *editable, gpointer data)
{
	EmployeeEditor *e = (EmployeeEditor *) data;

	if (!e->priv->selected)
		g_error("employee name changed with no employee selected");
	BackendSetEmployeeName(backend, &(e->priv->selectedIter), gtk_entry_get_text(GTK_ENTRY(e->name)));
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
	EmployeeEditor *e = (EmployeeEditor *) data;

	e->priv->selected = gtk_tree_selection_get_selected(e->listSelection, NULL, &(e->priv->selectedIter));
	gtk_widget_set_sensitive(e->name, e->priv->selected);
	gtk_widget_set_sensitive(e->changePassword, e->priv->selected);
	gtk_widget_set_sensitive(e->removeButton, e->priv->selected);
	if (e->priv->selected)
		gtk_entry_set_text(GTK_ENTRY(e->name), BackendEmployeeName(backend, &(e->priv->selectedIter)));
	else {
		// clear the fields to make deselection make sense
		// we need to disable our signal handlers to prevent the g_error() calls in our signal handlers from firing
		// TODO really necessary?
		g_signal_handler_block(e->name, e->priv->nameChangedHandler);
		gtk_entry_set_text(GTK_ENTRY(e->name), "");
		g_signal_handler_unblock(e->name, e->priv->nameChangedHandler);
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

GtkWidget *newEmployeeEditor(void)
{
	EmployeeEditor *e;

	e = makeEmployeeEditorFromUIFile();
	g_signal_connect(e->main, "destroy", G_CALLBACK(destroy), e);

	BackendSetEmployeesTreeView(backend, GTK_TREE_VIEW(e->list));
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(e->list), GTK_ENTRY(e->search));
	// TODO search should do case-insensitive substring or word-string match

	e->priv->nameChangedHandler = g_signal_connect(e->name, "changed", G_CALLBACK(nameChanged), e);
	g_signal_connect(e->changePassword, "clicked", G_CALLBACK(changePassword), e);
	g_signal_connect(e->listSelection, "changed", G_CALLBACK(selectionChanged), e);
	g_signal_connect(e->addButton, "clicked", G_CALLBACK(addEmployee), e);
	g_signal_connect(e->removeButton, "clicked", G_CALLBACK(removeEmployee), e);

	// get the ball rolling
	// TODO make this unnecessary
	selectionChanged(e->listSelection, e);

	return e->main;
}
