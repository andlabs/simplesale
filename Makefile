CFILES = items.c \
	main.c \
	order.c \
	paydialog.c \
	shift.c \
	util.c

HFILES = simplesale.h

ALLFILES = $(CFILES) $(HFILES)

all:
	gcc -g -o simplesale $(CFILES) -Wall -Wextra -pedantic --std=c99 `pkg-config --cflags --libs gtk+-3.0`
