// 13 june 2014
package main

import (
	"github.com/conformal/gotk3/gtk"
)

// _ returns for errors returned by conformal/gotk3; jrick regrets having the errors in and we both agreed they should be dropped

func myMain() {
	w, _ := gtk.WindowNew(gtk.WINDOW_TOPLEVEL)
	w.SetTitle("simplesale")
	w.Connect("delete-event", gtk.MainQuit)

	topbar, _ := gtk.HeaderBarNew()
	topbar.SetTitle("Total: $3.45")
	topbar.SetSubtitle("Subtotal: $2.34")
	topbar.SetShowCloseButton(false)
	w.SetTitlebar(topbar)

	cancelButton, _ := gtk.ButtonNewWithLabel("Cancel Order")
//	cancelButton.StyleContext.AddStyleClass("destructive-action")
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

func main() {
	gtk.Init(nil)
	myMain()
	gtk.Main()
}
