// 23 march 2015
// TODO version macros
#include <gtk/gtk.h>
#include <stdint.h>

enum {
	gtkMargin = 12,
	gtkXPadding = 12,
	gtkYPadding = 6,
};

// TODO prices

// manager.c
extern gboolean manager(void);

// addremovemoney.c
extern GtkWidget *newAddRemoveMoney(void);
