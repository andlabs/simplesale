CFILES = items.c \
	main.c \
	order.c \
	orderwin.c \
	paydialog.c \
	util.c

HFILES = simplesale.h

ALLFILES = $(CFILES) $(HFILES)

all:
	gcc -g -o simplesale $(CFILES) -Wall -Wextra -pedantic --std=c99 `pkg-config --cflags --libs gtk+-3.0`
