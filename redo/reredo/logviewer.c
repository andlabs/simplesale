// 24 march 2015
#include "simplesale.h"

#include "zlogviewer.h"

static void destroy(GtkWidget *widget, gpointer data)
{
	LogViewer *l = (LogViewer *) data;

	g_free(l);
}

GtkWidget *newLogViewer(void)
{
	LogViewer *l;

	l = makeLogViewerFromUIFile();
	g_signal_connect(l->main, "destroy", G_CALLBACK(destroy), l);

	GtkTextIter iter;

#define getEndIter() gtk_text_buffer_get_end_iter(l->logbuf, &iter)
#define append(t, c) gtk_text_buffer_insert_with_tags_by_name(l->logbuf, &iter, t, -1, c)
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

	return l->main;
}
