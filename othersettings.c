// 24 june 2014
#include "simplesale.h"

// no need to init or load as settings are grabbed as needed

typedef struct SaveOtherSettings SaveOtherSettings;

struct SaveOtherSettings {
	gboolean newManagerPassword;
	char *managerPassword;
};

static void saveOtherSettings(SaveOtherSettings *s)
{
	if (s->newManagerPassword)
;//TODO		setManagerPassword(s->managerPassword);
}

static void freeOtherSettings(SaveOtherSettings *s)
{
	g_free(s->managerPassword);
	g_free(s);
}

// the editor

typedef struct OtherSettings OtherSettings;
typedef struct OtherSettingsClass OtherSettingsClass;

struct OtherSettings {
	ManagerEditor parent_instance;

	GtkWidget *layout;
	GtkWidget *passentry;
	GtkWidget *change;

	SaveOtherSettings *newSettings;
};

struct OtherSettingsClass {
	ManagerEditorClass parent_class;
};

G_DEFINE_TYPE(OtherSettings, otherSettings, managerEditor_get_type())

static void buildOtherSettingsGUI(OtherSettings *);

static void otherSettings_init(OtherSettings *o)
{
	o->newSettings = (SaveOtherSettings *) g_malloc0(sizeof (SaveOtherSettings));
	buildOtherSettingsGUI(o);
}

static void otherSettings_dispose(GObject *obj)
{
	// no need to explicitly gtk_widget_destroy() anything
	G_OBJECT_CLASS(otherSettings_parent_class)->dispose(obj);
}

static void otherSettings_finalize(GObject *obj)
{
	OtherSettings *o = (OtherSettings *) obj;

	freeOtherSettings(o->newSettings);
	G_OBJECT_CLASS(otherSettings_parent_class)->finalize(obj);
}

static void otherSettings_class_init(OtherSettingsClass *class)
{
	G_OBJECT_CLASS(class)->dispose = otherSettings_dispose;
	G_OBJECT_CLASS(class)->finalize = otherSettings_finalize;
}

GtkWidget *newOtherSettings(void)
{
	return (GtkWidget *) g_object_new(otherSettings_get_type(), "type", GTK_WINDOW_TOPLEVEL, NULL);
}

static void saveClicked(GtkButton *button, gpointer data)
{
	USED(button);

	OtherSettings *o = (OtherSettings *) data;

	saveOtherSettings(o->newSettings);
	managerEditorDone(MANAGER_EDITOR(o));
}

static void discardClicked(GtkButton *button, gpointer data)
{
	USED(button);

	OtherSettings *o = (OtherSettings *) data;

	if (!askConfirm(GTK_WIDGET(o), NULL,
		"Are you sure you want to discard all changes and leave the Other Settings window? This will also discard password changes!"))
		return;
	managerEditorDone(MANAGER_EDITOR(o));
}

static void changeMPClicked(GtkButton *button, gpointer data)
{
	USED(button);

	OtherSettings *o = (OtherSettings *) data;
	const char *curpass;

	curpass = passEntryCurrentPassword(PASS_ENTRY(o->passentry));
	if (!matchesManagerPassword(curpass)) {
		passEntryNotifyFailure(PASS_ENTRY(o->passentry));
		return;
	}
	o->newSettings->newManagerPassword = TRUE;
	o->newSettings->managerPassword = g_strdup(passEntryNewPassword(PASS_ENTRY(o->passentry)));
	passEntryNotifyChanged(PASS_ENTRY(o->passentry));
}

void buildOtherSettingsGUI(OtherSettings *o)
{
	GtkWidget *topbar;
	GtkWidget *groupbox;
	GtkWidget *groupgrid;

	gtk_window_set_title(GTK_WINDOW(o), "simplesale");
	// TODO remove the following
	g_signal_connect(o, "delete-event", gtk_main_quit, NULL);

	// the initail height is too small
	expandWindowHeight(GTK_WINDOW(o), 2);

	topbar = newHeaderBar("Other Settings", GTK_WIDGET(o));
	newConfirmHeaderButton("Save and Close", G_CALLBACK(saveClicked), o, topbar);
	newCancelHeaderButton("Discard and Close", G_CALLBACK(discardClicked), o, topbar);

	o->layout = gtk_grid_new();

	groupbox = gtk_frame_new("Manager Password (changes when Change clicked)");
	groupgrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(groupbox), groupgrid);

	o->change = gtk_button_new_with_label("Change");
	g_signal_connect(o->change, "clicked", G_CALLBACK(changeMPClicked), o);
	gtk_widget_set_hexpand(o->change, FALSE);
	gtk_widget_set_halign(o->change, GTK_ALIGN_START);
	// don't attach; see what we do next

	o->passentry = newPassEntry(TRUE, o->change);
	gtk_widget_set_hexpand(o->passentry, TRUE);
	gtk_widget_set_halign(o->passentry, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(groupgrid),
		o->passentry, NULL,
		GTK_POS_TOP, 1, 1);

	gtk_widget_set_hexpand(groupbox, TRUE);
	gtk_widget_set_halign(groupbox, GTK_ALIGN_FILL);
	gtk_grid_attach_next_to(GTK_GRID(o->layout),
		groupbox, NULL,
		GTK_POS_TOP, 1, 1);

	gtk_container_add(GTK_CONTAINER(o), o->layout);
}
