// 24 march 2015
#include "simplesale.h"

#define LogViewerType (LogViewer_get_type())
#define LogViewer(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LogViewerType, LogViewer))
#define IsLogViewer(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LogViewerType))
#define LogViewerClass(class) (G_TYPE_CHECK_CLASS_CAST((class), LogViewerType, LogViewerClass))
#define IsLogViewerClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), LogViewer))
#define GetLogViewerClass(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), LogViewerType, LogViewerClass))
typedef struct LogViewer LogViewer;
typedef struct LogViewerClass LogViewerClass;
struct LogViewer {
	GtkGrid parent_instance;
	struct LogViewerPrivate *priv;
};
struct LogViewerClass {
	GtkGridClass parent_class;
};
static GType LogViewer_get_type(void);

typedef struct LogViewerPrivate LogViewerPrivate;

struct LogViewerPrivate {
	GtkTextBuffer *logbuf;
	GtkWidget *log;
	GtkWidget *amount;
};

G_DEFINE_TYPE_WITH_CODE(LogViewer, LogViewer, GTK_TYPE_GRID,
	G_ADD_PRIVATE(LogViewer))

static void LogViewer_init(LogViewer *l)
{
	l->priv = LogViewer_get_instance_private(l);

	gtk_widget_init_template(GTK_WIDGET(l));

	GtkTextIter iter;

#define getEndIter() gtk_text_buffer_get_end_iter(l->priv->logbuf, &iter)
#define append(t, c) gtk_text_buffer_insert_with_tags_by_name(l->priv->logbuf, &iter, t, -1, c, NULL)
	getEndIter();
	append("Test log entry 1\n", "black");
	getEndIter();
	append("Test log entry 2\n", "red");
	getEndIter();
	append("Test log entry 3\n", "green");
	getEndIter();
	append("Test log entry 4\n", "blue");
	getEndIter();
	append("Test log entry 5\n", "purple");
}

static void LogViewer_dispose(GObject *obj)
{
	G_OBJECT_CLASS(LogViewer_parent_class)->dispose(obj);
}

static void LogViewer_finalize(GObject *obj)
{
	G_OBJECT_CLASS(LogViewer_parent_class)->finalize(obj);
}

static void LogViewer_class_init(LogViewerClass *class)
{
	G_OBJECT_CLASS(class)->dispose = LogViewer_dispose;
	G_OBJECT_CLASS(class)->finalize = LogViewer_finalize;

	// TODO macroize filename somehow?
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/simplesale/logviewer.ui");
#define T(name) gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), LogViewer, name)
	T(logbuf);
	T(log);
	T(amount);
#undef T
}

GtkWidget *newLogViewer(void)
{
	return GTK_WIDGET(g_object_new(LogViewerType, NULL));
}
