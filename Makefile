CFILES = items.c \
	main.c \
	order.c \
	orderwin.c \
	util.c

HFILES = simplesale.h

ALLFILES = $(CFILES) $(HFILES)

all: $(ALLFILES)
	gcc -g -o simplesale $(CFILES) --std=c99 `pkg-config --cflags --libs gtk+-3.0`
