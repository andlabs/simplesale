VALAFILES= \
	addremovemoney.vala \
	db.vala \
	deviceeditor.vala \
	devicelist.vala \
	employeeeditor.vala \
	employees.vala \
	itemeditor.vala \
	items.vala \
	login.vala \
	main.vala \
	manager.vala \
	order.vala \
	orderwindow.vala \
	othersettings.vala \
	passwordui.vala \
	paydialog.vala \
	price.vala \
	shift.vala \
	shiftwindow.vala \
	usb.vala \
	usbids.vala \
	util.vala

VALAPKGS= \
	--vapidir . \
	--pkg gtk+-3.0 \
	--pkg pwquality \
	--pkg gusb -X -DG_USB_API_IS_SUBJECT_TO_CHANGE

RESOURCEXML = gresources.xml
RESOURCEC = zresources.c

VALARESOURCES= \
	--gresources $(RESOURCEXML) \
	--target-glib=2.38 \
	$(RESOURCEC)

all:
	jhbuild run glib-compile-resources --target $(RESOURCEC) --generate-source $(RESOURCEXML)
	jhbuild run valac -g $(VALAPKGS) $(VALAFILES) -o simplesale $(VALAFLAGS) $(VALARESOURCES)

c: VALAFLAGS += -C -H simplesale.h
c: all
C: c

clean:
	rm -f *.c *.h
