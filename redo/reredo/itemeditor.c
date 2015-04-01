// 24 march 2015
#include "simplesale.h"

// TODO selection changed does not fire when deleting the last item (which selects the new last item)...?
// or rather, selectedIter doesn't change?

#define ItemEditorType (ItemEditor_get_type())
#define ItemEditor(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ItemEditorType, ItemEditor))
#define IsItemEditor(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ItemEditorType))
#define ItemEditorClass(class) (G_TYPE_CHECK_CLASS_CAST((class), ItemEditorType, ItemEditorClass))
#define IsItemEditorClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), ItemEditor))
#define GetItemEditorClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), ItemEditorType, ItemEditorClass))
typedef struct ItemEditor ItemEditor;
typedef struct ItemEditorClass ItemEditorClass;
struct ItemEditor {
	GtkBox parent_instance;
	struct ItemEditorPrivate *priv;
};
struct ItemEditorClass {
	GtkBoxClass parent_class;
};
static GType ItemEditor_get_type(void);

typedef struct ItemEditorPrivate ItemEditorPrivate;

struct ItemEditorPrivate {
	gboolean selected;
	GtkTreeIter selectedIter;		// TODO rename selection?
	gulong nameChangedHandler;
	gulong priceChangedHandler;
	GtkWidget *list;
	GtkTreeSelection *listSelection;
	GtkWidget *addButton;
	GtkWidget *removeButton;
	GtkWidget *search;
	GtkWidget *name;
	GtkWidget *price;
};

G_DEFINE_TYPE_WITH_CODE(ItemEditor, ItemEditor, GTK_TYPE_BOX,
	G_ADD_PRIVATE(ItemEditor))

static void nameChanged(GtkEditable *editable, gpointer data)
{
	ItemEditor *e = ItemEditor(data);

	if (!e->priv->selected)
		g_error("item name changed with no item selected");
	BackendSetItemName(backend, &(e->priv->selectedIter), gtk_entry_get_text(GTK_ENTRY(e->priv->name)));
}

static void priceChanged(GtkEditable *editable, gpointer data)
{
	ItemEditor *e = ItemEditor(data);
	gboolean valid;
	Price price;

	if (!e->priv->selected)
		g_error("item name changed with no item selected");
	g_object_get(e->priv->price,
		"valid", &valid,
		"price", &price,
		NULL);
	if (valid)
		BackendSetItemPrice(backend, &(e->priv->selectedIter), price);
}

static void selectionChanged(GtkTreeSelection *sel, gpointer data)
{
	ItemEditor *e = ItemEditor(data);

	e->priv->selected = gtk_tree_selection_get_selected(e->priv->listSelection, NULL, &(e->priv->selectedIter));
	gtk_widget_set_sensitive(e->priv->name, e->priv->selected);
	gtk_widget_set_sensitive(e->priv->price, e->priv->selected);
	gtk_widget_set_sensitive(e->priv->removeButton, e->priv->selected);
	if (e->priv->selected) {
		// TODO provide a method to get both at once?
		gtk_entry_set_text(GTK_ENTRY(e->priv->name), BackendItemName(backend, &(e->priv->selectedIter)));
		g_object_set(e->priv->price, "price", BackendItemPrice(backend, &(e->priv->selectedIter)), NULL);
	} else {
		// clear the fields to make deselection make sense
		// we need to disable our signal handlers to prevent the g_error() calls in our signal handlers from firing
		// TODO really necessary?
		g_signal_handler_block(e->priv->name, e->priv->nameChangedHandler);
		g_signal_handler_block(e->priv->price, e->priv->priceChangedHandler);
		gtk_entry_set_text(GTK_ENTRY(e->priv->name), "");
		g_object_set(e->priv->price, "price", 0, NULL);
		g_signal_handler_unblock(e->priv->name, e->priv->nameChangedHandler);
		g_signal_handler_unblock(e->priv->price, e->priv->priceChangedHandler);
	}
}

static void addItem(GtkToolButton *b, gpointer data)
{
	BackendAppendItem(backend, "New Item", 0);
}

static void removeItem(GtkToolButton *b, gpointer data)
{
	ItemEditor *e = ItemEditor(data);

	if (!e->priv->selected)
		g_error("remove item button clicked without any item selected");
	BackendDeleteItem(backend, &(e->priv->selectedIter));
}

static void ItemEditor_init(ItemEditor *e)
{
	e->priv = ItemEditor_get_instance_private(e);

	gtk_widget_init_template(GTK_WIDGET(e));

	BackendSetItemsTreeView(backend, GTK_TREE_VIEW(e->priv->list));
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(e->priv->list), GTK_ENTRY(e->priv->search));
	// TODO search should do case-insensitive substring or word-string match

	e->priv->nameChangedHandler = g_signal_connect(e->priv->name, "changed", G_CALLBACK(nameChanged), e);
	// TODO signal on valid instead?
	e->priv->priceChangedHandler = g_signal_connect(e->priv->price, "changed", G_CALLBACK(priceChanged), e);
	g_signal_connect(e->priv->listSelection, "changed", G_CALLBACK(selectionChanged), e);
	g_signal_connect(e->priv->addButton, "clicked", G_CALLBACK(addItem), e);
	g_signal_connect(e->priv->removeButton, "clicked", G_CALLBACK(removeItem), e);

	// get the ball rolling
	// TODO make this unnecessary
	// TODO temporarily disabled due to PriceEntry GtkBuilder bug
//TODO	selectionChanged(e->priv->listSelection, e);
}

static void ItemEditor_dispose(GObject *obj)
{
	G_OBJECT_CLASS(ItemEditor_parent_class)->dispose(obj);
}

static void ItemEditor_finalize(GObject *obj)
{
	G_OBJECT_CLASS(ItemEditor_parent_class)->finalize(obj);
}

static void ItemEditor_class_init(ItemEditorClass *class)
{
	G_OBJECT_CLASS(class)->dispose = ItemEditor_dispose;
	G_OBJECT_CLASS(class)->finalize = ItemEditor_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/itemeditor.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemEditor, name)
	T(list);
	T(listSelection);
	T(addButton);
	T(removeButton);
	T(search);
	T(name);
	T(price);
#undef T
}

GtkWidget *newItemEditor(void)
{
	return GTK_WIDGET(g_object_new(ItemEditorType, NULL));
}
