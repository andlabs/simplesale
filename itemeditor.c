// 18 june 2014
#include "simplesale.h"

typedef struct ItemEditor ItemEditor;
typedef struct ItemEditorClass ItemEditorClass;

struct ItemEditor {
	GtkWindow parent_instance;

	GtkWidget *layout;
	GtkWidget *leftside;
	GtkWidget *search;
	GtkWidget *remove;
	GtkWidget *list;
	GtkTreeSelection *listSel;		// convenience
	GtkWidget *listScroller;
	GtkWidget *rightside;
	GtkWidget *name;
	GtkWidget *price;

	gboolean selecting;
};

struct ItemEditorClass {
	GtkWindowClass parent_class;
};

G_DEFINE_TYPE(ItemEditor, itemEditor, GTK_TYPE_WINDOW)

static void buildItemEditorGUI(ItemEditor *);

static void itemEditor_init(ItemEditor *e)
{
	buildItemEditorGUI(e);
}

static void itemEditor_dispose(GObject *obj)
{
	// no need to explicitly dispose ourselves
	G_OBJECT_CLASS(itemEditor_parent_class)->dispose(obj);
}

static void itemEditor_finalize(GObject *obj)
{
	G_OBJECT_CLASS(itemEditor_parent_class)->finalize(obj);
}

static guint itemEditorSignals[1];

static void itemEditor_class_init(ItemEditorClass *class)
{
	G_OBJECT_CLASS(class)->dispose = itemEditor_dispose;
	G_OBJECT_CLASS(class)->finalize = itemEditor_finalize;

	itemEditorSignals[0] = g_signal_new(
		"done", itemEditor_get_type(),
		G_SIGNAL_RUN_LAST,
		0,				// no class method slot
		NULL, NULL,		// no accumulator
		NULL,			// no marshaller
		G_TYPE_NONE,		// void clockOut(Shift *s, gpointer data);
		0);				// only specify the middle parameters; thanks larsu in irc.gimp.net/#gtk+
}

GtkWidget *newItemEditor(void)
{
	return (GtkWidget *) g_object_new(itemEditor_get_type(), "type", GTK_WINDOW_TOPLEVEL, NULL);
}

static void saveClicked(GtkButton *button, gpointer data)
{
	USED(button);

	ItemEditor *e = (ItemEditor *) data;

	saveItems();
	g_signal_emit(e, itemEditorSignals[0], 0);
}

static void discardClicked(GtkButton *button, gpointer data)
{
	USED(button);

	ItemEditor *e = (ItemEditor *) data;

	if (!askConfirm(GTK_WIDGET(e), NULL, "Are you sure you want to discard all changes and leave the Item Editor?"))
		return;
	initItems();
	g_signal_emit(e, itemEditorSignals[0], 0);
}

static void addItemClicked(GtkButton *button, gpointer data)
{
	USED(button);

	ItemEditor *e = (ItemEditor *) data;
	GtkTreeIter iter;

	iter = addItem("New Item", 0);
	gtk_tree_selection_select_iter(e->listSel, &iter);
}

static void removeItemClicked(GtkButton *button, gpointer data)
{
	USED(button);

	ItemEditor *e = (ItemEditor *) data;
	GtkTreeIter iter;
	char *name;

	if (gtk_tree_selection_get_selected(e->listSel, NULL, &iter) == FALSE)
		g_error("Delete Item button clicked without any item selected (button should be disabled)");
	getItem(&iter, &name, NULL);
	if (!askConfirm(GTK_WIDGET(e),
		"Once you click Save and Close in the Item Editor window, the item will be permanently deleted and cannot be recovered later.",
		"Are you sure you want to delete \"%s\"?", name))
		return;
	deleteItem(&iter);
}

static void itemSelected(GtkTreeSelection *selection, gpointer data)
{
	USED(selection);

	ItemEditor *e = (ItemEditor *) data;
	gboolean selected;
	char *name = "";
	Price price;
	GtkTreeIter iter;

	selected = gtk_tree_selection_get_selected(e->listSel, NULL, &iter);
	gtk_widget_set_sensitive(e->remove, selected);
	gtk_widget_set_sensitive(e->name, selected);
	gtk_widget_set_sensitive(e->price, selected);
	e->selecting = TRUE;
	if (selected) {
		getItem(&iter, &name, &price);
		priceEntrySetPrice(PRICE_ENTRY(e->price), price);
	} else
		priceEntryClear(PRICE_ENTRY(e->price));
	gtk_entry_set_text(GTK_ENTRY(e->name), name);
	e->selecting = FALSE;

	// informational
	if (selected)
		gtk_entry_set_placeholder_text(GTK_ENTRY(e->name), "Enter item name here and item price below.");
	else
		gtk_entry_set_placeholder_text(GTK_ENTRY(e->name), "Select an item at left or click + to add a new item.");
}

static void nameChanged(GtkEditable *editable, gpointer data)
{
	USED(editable);

	ItemEditor *e = (ItemEditor *) data;
	GtkTreeIter iter;

	if (e->selecting)	// prevent spurious g_error() during selection changed
		return;
	if (gtk_tree_selection_get_selected(e->listSel, NULL, &iter) == FALSE)
		g_error("item changed without any item selected (textbox should be disabled)");
	setItemName(&iter, gtk_entry_get_text(GTK_ENTRY(e->name)));
}

static void priceChanged(GtkEditable *editable, gpointer data)
{
	USED(editable);

	ItemEditor *e = (ItemEditor *) data;
	GtkTreeIter iter;
	Price price;

	if (e->selecting)	// prevent spurious g_error() during selection changed
		return;
	if (gtk_tree_selection_get_selected(e->listSel, NULL, &iter) == FALSE)
		g_error("item changed without any item selected (textbox should be disabled)");
	if (priceEntryGetPrice(PRICE_ENTRY(e->price), &price) != FALSE)
		setItemPrice(&iter, price);
}

static void buildItemEditorGUI(ItemEditor *e)
{
	GtkWidget *topbar;
	GtkWidget *button;

	gtk_window_set_title(GTK_WINDOW(e), "simplesale");
	// TODO remove the following
	g_signal_connect(e, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	expandWindowHeight(GTK_WINDOW(e), 2);

	topbar = newHeaderBar("Item Editor", GTK_WIDGET(e));
	newConfirmHeaderButton("Save and Close", G_CALLBACK(saveClicked), e, topbar);
	newCancelHeaderButton("Discard and Close", G_CALLBACK(discardClicked), e, topbar);

	e->leftside = gtk_grid_new();
	e->search = gtk_search_entry_new();
	// no live search; we assign the interactive search widget of the GtkTreeView below to this
	// TODO that gets rid of typing in the treeview...
	gtk_widget_set_hexpand(e->search, TRUE);
	gtk_widget_set_halign(e->search, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		e->search, NULL,
		GTK_POS_TOP, 1, 1);

	button = gtk_button_new_from_icon_name("list-add", GTK_ICON_SIZE_BUTTON);
	// TODO style class?
	g_signal_connect(button, "clicked", G_CALLBACK(addItemClicked), e);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		button, e->search,
		GTK_POS_RIGHT, 1, 1);

	e->remove = gtk_button_new_from_icon_name("list-remove", GTK_ICON_SIZE_BUTTON);
	// TODO style class?
	g_signal_connect(e->remove, "clicked", G_CALLBACK(removeItemClicked), e);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		e->remove, button,
		GTK_POS_RIGHT, 1, 1);

	e->list = gtk_tree_view_new();
	setItemsModel(GTK_TREE_VIEW(e->list));
	setItemsColumnLayout(GTK_TREE_VIEW(e->list));
	e->listSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(e->list));
	// TODO figure out how to make it so that clicking on blank space deselects
	g_signal_connect(e->listSel, "changed", G_CALLBACK(itemSelected), e);
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(e->list), GTK_ENTRY(e->search));
	e->listScroller = newListScroller(e->list);
	gtk_grid_attach_next_to(GTK_GRID(e->leftside),
		e->listScroller, e->search,
		GTK_POS_BOTTOM, 4, 1);

	e->rightside = gtk_grid_new();

	e->name = gtk_entry_new();
	g_signal_connect(e->name, "changed", G_CALLBACK(nameChanged), e);
	gtk_widget_set_hexpand(e->name, TRUE);
	gtk_widget_set_halign(e->name, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->name, NULL,
		GTK_POS_RIGHT, 1, 1);
	attachLabel("Name:", e->name, e->rightside);

	e->price = newPriceEntry();
	priceEntryConnect(PRICE_ENTRY(e->price), "changed", G_CALLBACK(priceChanged), e);
	gtk_grid_attach_next_to(GTK_GRID(e->rightside),
		e->price, e->name,
		GTK_POS_BOTTOM, 1, 1);
	attachLabel("Price:", e->price, e->rightside);

	// TODO add an undo button?

	e->layout = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(e->layout), TRUE);
	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->leftside, NULL,
		GTK_POS_TOP, 1, 1);
	gtk_grid_attach_next_to(GTK_GRID(e->layout),
		e->rightside, e->leftside,
		GTK_POS_RIGHT, 2, 1);

	gtk_container_add(GTK_CONTAINER(e), e->layout);
	gtk_widget_show_all(GTK_WIDGET(e));

	itemSelected(NULL, e);		// set up initial state
}

void itemEditorShow(ItemEditor *e)
{
	gtk_widget_show_all(GTK_WIDGET(e));
}
