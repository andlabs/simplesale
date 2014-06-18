CFILES = db.c \
	items.c \
	itemeditor.c \
	main.c \
	login.c \
	order.c \
	paydialog.c \
	price.c \
	shift.c \
	util.c

HFILES = simplesale.h

ALLFILES = $(CFILES) $(HFILES)

all:
	gcc -g -o simplesale $(CFILES) -Wall -Wextra -pedantic --std=c99 `pkg-config --cflags --libs gtk+-3.0` -lpwquality
