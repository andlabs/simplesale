// 14 june 2014
package main

import (
	"flag"
)

// /* TODO THESE ARE TO DEAL WITH INCOMPLETENESS IN GOTK3 */
// #cgo  pkg-config: gtk+-3.0
// #include "simplesale.h"
// void setTheme(char *x)
// {
// 	g_object_set(gtk_settings_get_default(),
// 		"gtk-theme-name", x,
// 		NULL);
// 	free(x);
// }
import "C"

var gtkTheme = flag.String("theme", "", "if set, GTK+ theme to use")

func main() {
	C.gtk_init(nil, nil)
	flag.Parse()
	if *gtkTheme != "" {
		C.setTheme(C.CString(*gtkTheme))
	}
	C.newOrderWindow()
	C.gtk_main()
}
