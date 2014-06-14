// 13 june 2014
package main

import (
	"flag"
	"github.com/conformal/gotk3/gtk"
)

// /* TODO THESE ARE TO DEAL WITH INCOMPLETENESS IN GOTK3 */
// #cgo  pkg-config: gtk+-3.0
// #include <gtk/gtk.h>
// #include <stdint.h>
// #include <stdlib.h>
// void makeSuggestedAction(uintptr_t x)
// {
// 	gtk_style_context_add_class(
// 		gtk_widget_get_style_context((GtkWidget *) x),
// 		"suggested-action");
// }
// void makeDestructiveAction(uintptr_t x)
// {
// 	gtk_style_context_add_class(
// 		gtk_widget_get_style_context((GtkWidget *) x),
// 		"destructive-action");
// }
// void setTheme(char *x)
// {
// 	g_object_set(gtk_settings_get_default(),
// 		"gtk-theme-name", x,
// 		NULL);
// 	free(x);
// }
import "C"

// _ returns for errors returned by conformal/gotk3; jrick regrets having the errors in and we both agreed they should be dropped

func myMain() {
	w, _ := gtk.WindowNew(gtk.WINDOW_TOPLEVEL)
	w.SetTitle("simplesale")
	w.Connect("delete-event", gtk.MainQuit)

	// the initail height is too small
	width, height := w.GetDefaultSize()
	if height == -1 {
		_, height = w.GetSize()
	}
	w.SetDefaultSize(width, height * 3)

	topbar, _ := gtk.HeaderBarNew()
	topbar.SetTitle("Total: $3.45")
	topbar.SetSubtitle("Subtotal: $2.34")
	topbar.SetShowCloseButton(false)
	w.SetTitlebar(topbar)

	payNowButton, _ := gtk.ButtonNewWithLabel("Pay Now")
//	payNowButton.StyleContext.AddClass("suggested-action")
	C.makeSuggestedAction(C.uintptr_t(payNowButton.Native()))
	topbar.PackStart(payNowButton)
	payLaterButton, _ := gtk.ButtonNewWithLabel("Pay Later")
//	payLaterButton.StyleContext.AddClass("suggested-action")
	C.makeSuggestedAction(C.uintptr_t(payLaterButton.Native()))
	topbar.PackStart(payLaterButton)

	cancelButton, _ := gtk.ButtonNewWithLabel("Cancel Order")
//	cancelButton.StyleContext.AddClass("destructive-action")
	C.makeDestructiveAction(C.uintptr_t(cancelButton.Native()))
	topbar.PackEnd(cancelButton)

	layout, _ := gtk.GridNew()

	leftside, _ := gtk.GridNew()
	customerLabel, _ := gtk.LabelNew("Customer:")
	leftside.Attach(
		customerLabel,
		0, 0, 1, 1)
//		nil,
//		gtk.POS_TOP, 1, 1)
	customerName, _ := gtk.EntryNew()
	customerName.SetHExpand(true)
	customerName.SetHAlign(gtk.ALIGN_FILL)
	leftside.AttachNextTo(
		customerName,
		customerLabel,
		gtk.POS_RIGHT, 1, 1)
	order, _ := gtk.TreeViewNew()
	orderScroller, _ := gtk.ScrolledWindowNew(nil, nil)
	orderScroller.Add(order)
//	orderScroller.SetShadowType(gtk.SHADOW_IN)
	orderScroller.SetHExpand(true)
	orderScroller.SetHAlign(gtk.ALIGN_FILL)
	orderScroller.SetVExpand(true)
	orderScroller.SetVAlign(gtk.ALIGN_FILL)
	leftside.AttachNextTo(
		orderScroller,
		customerLabel,
		gtk.POS_BOTTOM, 2, 1)

	rightside, _ := gtk.GridNew()
	rightside.SetColumnHomogeneous(true)
	searchBox, _ := gtk.SearchEntryNew()
	searchBox.SetHExpand(true)
	searchBox.SetHAlign(gtk.ALIGN_FILL)
	rightside.Attach(
		searchBox,
		0, 0, 1, 1)
//		nil,
//		gtk.POS_TOP, 1, 1)
	items, _ := gtk.TreeViewNew()
	itemsScroller, _ := gtk.ScrolledWindowNew(nil, nil)
	itemsScroller.Add(items)
//	itemsScroller.SetShadowType(gtk.SHADOW_IN)
	itemsScroller.SetHExpand(true)
	itemsScroller.SetHAlign(gtk.ALIGN_FILL)
	itemsScroller.SetVExpand(true)
	itemsScroller.SetVAlign(gtk.ALIGN_FILL)
	rightside.AttachNextTo(
		itemsScroller,
		searchBox,
		gtk.POS_BOTTOM, 2, 1)

	layout.SetColumnHomogeneous(true)
	leftside.SetHExpand(true)
	leftside.SetHAlign(gtk.ALIGN_FILL)
	leftside.SetVExpand(true)
	leftside.SetVAlign(gtk.ALIGN_FILL)
	layout.Attach(
		leftside,
		0, 0, 1, 1)
//		nil,
//		gtk.POS_TOP, 1, 1)
	rightside.SetHExpand(true)
	rightside.SetHAlign(gtk.ALIGN_FILL)
	rightside.SetVExpand(true)
	rightside.SetVAlign(gtk.ALIGN_FILL)
	layout.AttachNextTo(
		rightside,
		leftside,
		gtk.POS_RIGHT, 2, 1)

	w.Add(layout)
	w.ShowAll()
}

var gtkTheme = flag.String("theme", "", "if set, GTK+ theme to use")

func main() {
	gtk.Init(nil)
	flag.Parse()
	if *gtkTheme != "" {
		C.setTheme(C.CString(*gtkTheme))
	}
	myMain()
	gtk.Main()
}
