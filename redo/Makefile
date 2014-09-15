VALAFILES= \
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
	passwordui.vala \
	paydialog.vala \
	price.vala \
	shift.vala \
	shiftwindow.vala \
	usbids.vala \
	util.vala

VALAPKGS= \
	--vapidir . \
	--pkg gtk+-3.0 \
	--pkg pwquality \
	--pkg gusb -X -DG_USB_API_IS_SUBJECT_TO_CHANGE

RESOURCES= \
	--gresources gresources.xml

all:
	jhbuild run valac -g $(VALAPKGS) $(VALAFILES) -o simplesale $(VALAFLAGS) $(RESOURCES)

c: VALAFLAGS += -C -H simplesale.h
c: all
C: c

clean:
	rm -f *.c *.h
