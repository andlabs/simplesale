// 15 june 2014
#include "simplesale.h"

struct Shift {
	GObject parent_instance;

	char *employee;
	GHashTable *orders;

	GtkWidget *win;
	GtkWidget *topbar;
	GtkWidget *layout;
	GtkListStore *saved;
	GtkWidget *list;
	GtkTreeSelection *listSel;
	GtkWidget *listScroller;
	GtkWidget *resume;
};

typedef struct ShiftClass ShiftClass;

struct ShiftClass {
	GObjectClass parent_class;
};

G_DEFINE_TYPE(Shift, shift, G_TYPE_OBJECT)

static void buildShiftGUI(Shift *);
static void freeShiftGUI(Shift *);

static void shift_init(Shift *s)
{
	s->orders = g_hash_table_new(g_direct_hash, g_direct_equal);
	buildShiftGUI(s);
}

static void shift_dispose(GObject *obj)
{
	Shift *s = (Shift *) obj;

	freeShiftGUI(s);
	g_hash_table_unref(s->orders);
	G_OBJECT_CLASS(shift_parent_class)->dispose(obj);
}

static void shift_finalize(GObject *obj)
{
	Shift *s = (Shift *) obj;

	g_free(s->employee);
	G_OBJECT_CLASS(shift_parent_class)->finalize(obj);
}

static GParamSpec *shiftProperties[2];
static guint shiftSignals[1];

// cheat with these two; we only have one property

static void shift_setProperty(GObject *obj, guint id, const GValue *value, GParamSpec *pspec)
{
	USED(id);
	USED(pspec);

	Shift *s = (Shift *) obj;

	g_free(s->employee);
	s->employee = g_value_dup_string(value);
	gtk_header_bar_set_title(GTK_HEADER_BAR(s->topbar), s->employee);
}

static void shift_getProperty(GObject *obj, guint id, GValue *value, GParamSpec *pspec)
{
	USED(id);
	USED(pspec);

	Shift *s = (Shift *) obj;

	g_value_set_string(value, s->employee);
}

static void shift_class_init(ShiftClass *class)
{
	G_OBJECT_CLASS(class)->dispose = shift_dispose;
	G_OBJECT_CLASS(class)->finalize = shift_finalize;
	G_OBJECT_CLASS(class)->set_property = shift_setProperty;
	G_OBJECT_CLASS(class)->get_property = shift_getProperty;

	shiftProperties[1] = g_param_spec_string(
		"employee",
		"Employee name",
		"Employee name",
		"<unnamed employee>",				// TODO
		G_PARAM_CONSTRUCT_ONLY |		// don't change later
			G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
	g_object_class_install_properties(G_OBJECT_CLASS(class), 2, shiftProperties);

	shiftSignals[0] = g_signal_new(
		"clock-out", shift_get_type(),
		G_SIGNAL_RUN_LAST,
		0,				// no class method slot
		NULL, NULL,		// no accumulator
		NULL,			// no marshaller
		G_TYPE_NONE,		// void clockOut(Shift *s, gpointer data);
		0);				// only specify the middle parameters; thanks larsu in irc.gimp.net/#gtk+
}

Shift *newShift(char *name)
{
	return (Shift *) g_object_new(shift_get_type(), "employee", name, NULL);
}

static void newOrderClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Shift *s = (Shift *) data;

	shiftNewOrder(s);
}

static void clockOutClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Shift *s = (Shift *) data;

	// TODO message box
	g_signal_emit(s, shiftSignals[0], 0);
}

static void adjustResumeEnabled(GtkTreeSelection *selection, gpointer data)
{
	USED(selection);

	Shift *s = (Shift *) data;

	gtk_widget_set_sensitive(s->resume,
		gtk_tree_selection_count_selected_rows(s->listSel) != 0);
}

static void resumeClicked(GtkButton *button, gpointer data)
{
	USED(button);

	Shift *s = (Shift *) data;
	GtkTreeIter iter;
	Order *o;

	if (gtk_tree_selection_get_selected(s->listSel, NULL, &iter) == FALSE)
		g_error("Resume Order button clicked without any order selected (button should be disabled)");
	gtk_tree_model_get(GTK_TREE_MODEL(s->saved), &iter, 1, &o, -1);
	orderShowWindow(o);
	gtk_list_store_remove(s->saved, &iter);
}

static void buildShiftGUI(Shift *s)
{
	gint width, height;
	GtkWidget *button;
	GtkWidget *label;
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;

	s->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(s->win), "simplesale");
	// TODO remove the following
	g_signal_connect(s->win, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	gtk_window_get_default_size(GTK_WINDOW(s->win), &width, &height);
	if (height == -1)
		gtk_window_get_size(GTK_WINDOW(s->win), NULL, &height);
	gtk_window_set_default_size(GTK_WINDOW(s->win), width, height * 2);

	s->topbar = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(s->topbar), s->employee);
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(s->topbar), FALSE);
	gtk_window_set_titlebar(GTK_WINDOW(s->win), s->topbar);

	button = gtk_button_new_with_label("New Order");
	gtk_style_context_add_class(gtk_widget_get_style_context(button), "suggested-action");
	g_signal_connect(button, "clicked", G_CALLBACK(newOrderClicked), s);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(s->topbar), button);

	button = gtk_button_new_with_label("Clock Out");
	gtk_style_context_add_class(gtk_widget_get_style_context(button), "destructive-action");
	g_signal_connect(button, "clicked", G_CALLBACK(clockOutClicked), s);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(s->topbar), button);

	s->layout = gtk_grid_new();

	label = gtk_label_new("Orders for later:");
	alignLabel(label, 0);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(s->layout),
		label, NULL,
		GTK_POS_TOP, 1, 1);

	s->saved = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	s->list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(s->saved));
	s->listSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(s->list));
	// TODO figure out how to make it so that clicking on blank space deselects
	g_signal_connect(s->listSel, "changed", G_CALLBACK(adjustResumeEnabled), s);
	r = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("", r, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(s->list), col);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(s->list), FALSE);
	s->listScroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(s->listScroller), s->list);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(s->listScroller), GTK_SHADOW_IN);
	gtk_widget_set_hexpand(s->listScroller, TRUE);
	gtk_widget_set_halign(s->listScroller, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(s->listScroller, TRUE);
	gtk_widget_set_valign(s->listScroller, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(s->layout),
		s->listScroller, label,
		GTK_POS_BOTTOM, 1, 1);

	s->resume = gtk_button_new_with_label("Resume Order");
	gtk_widget_set_sensitive(s->resume, FALSE);		// initial state; no items present
	g_signal_connect(s->resume, "clicked", G_CALLBACK(resumeClicked), s);
	gtk_widget_set_hexpand(s->resume, TRUE);
	gtk_widget_set_halign(s->resume, GTK_ALIGN_END);
	gtk_grid_attach_next_to(GTK_GRID(s->layout),
		s->resume, s->listScroller,
		GTK_POS_BOTTOM, 1, 1);

	gtk_container_add(GTK_CONTAINER(s->win), s->layout);
}

static void freeShiftGUI(Shift *s)
{
	gtk_widget_destroy(s->win);		// TODO does this destroy subwidgets?
	g_object_unref(s->saved);
}

static void shiftDoOrder(Order *o, gint action, gpointer data)
{
	Shift *s = (Shift *) data;
	GtkTreeIter iter;

	switch (action) {
	case orderCancel:
		printf("order cancelled\n");
		break;
	case orderPayNow:
		printf("order paid now\n");
		break;
	case orderPayLater:
		gtk_list_store_append(s->saved, &iter);
		gtk_list_store_set(s->saved, &iter, 0, orderCustomer(o), 1, o, -1);
		orderHideWindow(o);
		return;		// DON'T FALL THROUGH! we still need the order!
	}

	g_hash_table_remove(s->orders, o);
	freeOrder(o);
}

void shiftNewOrder(Shift *s)
{
	Order *o;

	o = newOrder();
	g_signal_connect(o, "do", G_CALLBACK(shiftDoOrder), s);
	// sample items
	{
		addToOrder(o, 0);
		addToOrder(o, 1);
	}
	g_hash_table_insert(s->orders, o, NULL);
}

void shiftShowWindow(Shift *s)
{
	gtk_widget_show_all(s->win);
}

void shiftHideWindow(Shift *s)
{
	gtk_widget_hide(s->win);
}
