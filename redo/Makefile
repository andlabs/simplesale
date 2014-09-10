VALAFILES= \
	db.vala \
	employees.vala \
	items.vala \
	login.vala \
	main.vala \
	order.vala \
	orderwindow.vala \
	passwordui.vala \
	paydialog.vala \
	price.vala \
	shift.vala \
	shiftwindow.vala \
	util.vala

all:
	jhbuild run valac -g --pkg gtk+-3.0 $(VALAFILES) -o simplesale $(VALAFLAGS)

c: VALAFLAGS += -C -H simplesale.h
c: all
C: c

clean:
	rm *.c *.h
