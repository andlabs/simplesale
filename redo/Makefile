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
	shiftwindow.vala

all:
	jhbuild run valac -g --pkg gtk+-3.0 $(VALAFILES) -o simplesale $(VALAFLAGS)
