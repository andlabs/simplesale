VALAFILES= \
	db.vala \
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
	--pkg pwquality

all:
	jhbuild run valac -g $(VALAPKGS) $(VALAFILES) -o simplesale $(VALAFLAGS)

c: VALAFLAGS += -C -H simplesale.h
c: all
C: c

clean:
	rm -f *.c *.h
