// 20 june 2014
#include "simplesale.h"
#include <pwquality.h>

struct PassEntry {
	GtkGrid parent_instance;

	GtkWidget *curpass;
	GtkWidget *newpass;
	GtkWidget *passlevel;
	GtkWidget *passleveltext;
	GtkWidget *confirmpass;
	GtkWidget *child;

	gboolean askCurrent;
	char *name;
};

struct PassEntryClass {
	GtkGridClass parent_class;
};

G_DEFINE_TYPE(PassEntry, passEntry, GTK_TYPE_GRID)

static void refresh(PassEntry *, gboolean);

static void passwordChanged(GtkEditable *editable, gpointer data)
{
	USED(editable);

	PassEntry *e = (PassEntry *) data;
	const char *cur = NULL;
	const char *new, *confirm;
	int level;

	if (e->curpass != NULL)
		cur = gtk_entry_get_text(GTK_ENTRY(e->curpass));
	new = gtk_entry_get_text(GTK_ENTRY(e->newpass));
	confirm = gtk_entry_get_text(GTK_ENTRY(e->confirmpass));
	level = pwquality_check(pwquality_default_settings(), new, cur, e->name, NULL);
	if (level >= 0)
		gtk_level_bar_set_value(GTK_LEVEL_BAR(e->passlevel), (gdouble) level);
	if (level < 0) {
		gtk_level_bar_set_value(GTK_LEVEL_BAR(e->passlevel), 0);
		gtk_label_set_text(GTK_LABEL(e->passleveltext), pwquality_strerror(NULL, 0, level, NULL));
	} else if (strcmp(new, confirm) != 0)
		gtk_label_set_text(GTK_LABEL(e->passleveltext), "Passwords do not match.");
	else
		gtk_label_set_text(GTK_LABEL(e->passleveltext), "");
	refresh(e, FALSE);
}

static void passEntry_init(PassEntry *e)
{
	// askCurrent is handled during setProperties below

	e->newpass = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(e->newpass), FALSE);
	g_signal_connect(e->newpass, "changed", G_CALLBACK(passwordChanged), e);
	gtk_grid_attach_next_to(GTK_GRID(e),
		e->newpass, NULL,
		GTK_POS_BOTTOM, 1, 1);
	attachLabel("New Password:", e->newpass, GTK_WIDGET(e));

	e->confirmpass = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(e->confirmpass), FALSE);
	gtk_entry_set_placeholder_text(GTK_ENTRY(e->confirmpass), "Retype new password here");
	g_signal_connect(e->confirmpass, "changed", G_CALLBACK(passwordChanged), e);
	gtk_widget_set_hexpand(e->confirmpass, TRUE);
	gtk_widget_set_halign(e->confirmpass, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e),
		e->confirmpass, e->newpass,
		GTK_POS_RIGHT, 1, 1);

	e->passlevel = gtk_level_bar_new_for_interval(0, 100);
	gtk_widget_set_vexpand(e->passlevel, FALSE);
	gtk_widget_set_valign(e->passlevel, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e),
		e->passlevel, e->newpass,
		GTK_POS_BOTTOM, 1, 1);
	attachLabel("Password Strength:", e->passlevel, GTK_WIDGET(e));

	e->passleveltext = gtk_label_new("");
	alignLabel(e->passleveltext, 0);
	gtk_label_set_line_wrap(GTK_LABEL(e->passleveltext), TRUE);
	gtk_widget_set_hexpand(e->passleveltext, TRUE);
	gtk_widget_set_halign(e->passleveltext, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(e->passleveltext, FALSE);
	gtk_widget_set_valign(e->passleveltext, GTK_ALIGN_START);
	gtk_grid_attach_next_to(GTK_GRID(e),
		e->passleveltext, e->passlevel,
		GTK_POS_RIGHT, 1, 2);

	// child is handled below in setProperty
}

static void passEntry_dispose(GObject *obj)
{
	// don't unref the member widgets; they'll be unref-ed by the GtkGrid dispose()
	G_OBJECT_CLASS(passEntry_parent_class)->dispose(obj);
}

static void passEntry_finalize(GObject *obj)
{
	G_OBJECT_CLASS(passEntry_parent_class)->finalize(obj);
}

// 1 - ask-current (set only, construct only)
// 2 - child (set only, construct only)
// 3 - child-sensitive (get only, bindable)
// 4 - account-name (set only, bindable)
static GParamSpec *passEntryProperties[5];

static void passEntry_setProperty(GObject *obj, guint id, const GValue *value, GParamSpec *pspec)
{
	PassEntry *e = (PassEntry *) obj;

	switch (id) {
	case 1:		// ask-current
		e->askCurrent = g_value_get_boolean(value);
		if (e->askCurrent) {
			e->curpass = gtk_entry_new();
			gtk_entry_set_visibility(GTK_ENTRY(e->curpass), FALSE);
			g_signal_connect(e->curpass, "changed", G_CALLBACK(passwordChanged), e);
			gtk_widget_set_vexpand(e->curpass, FALSE);
			gtk_widget_set_valign(e->curpass, GTK_ALIGN_START);
			gtk_grid_attach_next_to(GTK_GRID(e),
				e->curpass, NULL,
					GTK_POS_TOP, 1, 1);
			attachLabel("Current Password:\n(if changing)", e->curpass, GTK_WIDGET(e));
		}
		break;
	case 2:		// child
		e->child = (GtkWidget *) g_value_get_object(value);
		if (e->child != NULL) {
			gtk_grid_attach_next_to(GTK_GRID(e),
				e->child, e->passlevel,
				GTK_POS_BOTTOM, 1, 1);
			g_object_bind_property(e, "child-sensitive",
				e->child, "sensitive",
				G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);
		}
		break;
	case 4:		// account-name
		g_free(e->name);
		e->name = g_value_dup_string(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(e, id, pspec);
	}
	refresh(e, TRUE);
}

static gboolean shouldEnable(PassEntry *e)
{
	const char *x;

	// disable if whole grid disabled
	if (gtk_widget_get_sensitive(GTK_WIDGET(e)) == FALSE)
		return FALSE;
	// enable if no warning text
	x = gtk_label_get_text(GTK_LABEL(e->passleveltext));
	return x == NULL || *x == '\0';
}

static void passEntry_getProperty(GObject *obj, guint id, GValue *value, GParamSpec *pspec)
{
	USED(value);

	PassEntry *e = (PassEntry *) obj;

	if (id == 3) {		// child-sensitive
		g_value_set_boolean(value, shouldEnable(e));
		return;
	}
	G_OBJECT_WARN_INVALID_PROPERTY_ID(e, id, pspec);
}

static void refresh(PassEntry *e, gboolean includePasswordChanged)
{
	if (includePasswordChanged)
		passwordChanged(NULL, e);
	g_signal_emit_by_name(e, "notify::child-sensitive", passEntryProperties[3]);
}

static void passEntry_class_init(PassEntryClass *class)
{
	G_OBJECT_CLASS(class)->dispose = passEntry_dispose;
	G_OBJECT_CLASS(class)->finalize = passEntry_finalize;
	G_OBJECT_CLASS(class)->set_property = passEntry_setProperty;
	G_OBJECT_CLASS(class)->get_property = passEntry_getProperty;

	passEntryProperties[1] = g_param_spec_boolean(
		"ask-current",
		"Ask for Current Password",
		"Adds a slot for asking for the current password. This will also affect the password quality check.",
		FALSE,
		G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);
	passEntryProperties[2] = g_param_spec_object(
		"child",
		"Child Widget",
		"An optional child widget. It will have its sensitivity automatically bound to the PassEntry's ::child-sensitive property.",
		GTK_TYPE_WIDGET,
		G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);
	passEntryProperties[3] = g_param_spec_boolean(
		"child-sensitive",
		"Child Sensitivity",
		"Returns TRUE if the child of this widget should be sensistive. Use this property directly to control widgets that aren't children. The widget's ::child will automatically be bound to this property.",
		FALSE,
		G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
	passEntryProperties[4] = g_param_spec_string(
		"account-name",
		"Real name of account holder",
		"Stores the real name of the account holder. Affects password quality calculations. Bind this to your GtkEntry.",
		NULL,
		G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);
	g_object_class_install_properties(G_OBJECT_CLASS(class), 5, passEntryProperties);
}

GtkWidget *newPassEntry(gboolean askCurrent, GtkWidget *child)
{
	return (GtkWidget *) g_object_new(passEntry_get_type(),
		"ask-current", askCurrent,
		"child", child,
		NULL);
}

void resetPassEntry(PassEntry *e)
{
	gtk_entry_set_text(GTK_ENTRY(e->curpass), "");
	gtk_entry_set_text(GTK_ENTRY(e->newpass), "");
	gtk_entry_set_text(GTK_ENTRY(e->confirmpass), "");
	gtk_level_bar_set_value(GTK_LEVEL_BAR(e->passlevel), 0);
	// look nice if disabled
	if (gtk_widget_get_sensitive(GTK_WIDGET(e)) == FALSE)
		gtk_label_set_text(GTK_LABEL(e->passleveltext), "");
}

const char *passEntryCurrentPassword(PassEntry *e)
{
	if (!shouldEnable(e))
		g_error("passEntryCurrentPassword() called when it should not be; control that triggered it should be disabled");
	return gtk_entry_get_text(GTK_ENTRY(e->curpass));
}

const char *passEntryNewPassword(PassEntry *e)
{
	if (!shouldEnable(e))
		g_error("passEntryNewPassword() called when it should not be; control that triggered it should be disabled");
	return gtk_entry_get_text(GTK_ENTRY(e->newpass));
}

char *askNewPassword(GtkWidget *parent, char *message, char *action)
{
	GtkWidget *pwprompt;
	GtkWidget *passentry;
	GtkBox *area;
	gint response;
	char *pw = NULL;

	pwprompt = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
		"%s", message);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(pwprompt),
		"Enter a password below and press OK to %s.", action);
	gtk_message_dialog_set_image(GTK_MESSAGE_DIALOG(pwprompt),
		gtk_image_new_from_icon_name("dialog-password", GTK_ICON_SIZE_DIALOG));			// size used by GtkMessageDialog already, so eh
	passentry = newPassEntry(FALSE, NULL);
	area = GTK_BOX(gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(pwprompt)));
	gtk_box_pack_end(area, passentry, TRUE, TRUE, 0);
	g_object_bind_property(passentry, "child-sensitive",
		gtk_dialog_get_widget_for_response(GTK_DIALOG(pwprompt), GTK_RESPONSE_OK), "sensitive",
		G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);
	gtk_widget_show_all(pwprompt);
	response = gtk_dialog_run(GTK_DIALOG(pwprompt));
	if (response == GTK_RESPONSE_OK)
		pw = g_strdup(passEntryNewPassword(PASS_ENTRY(passentry)));
	gtk_widget_destroy(pwprompt);
	return pw;
}
