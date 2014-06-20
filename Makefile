CFILES = accounts.c \
	db.c \
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

# the order here is specifically calculated
# note that -lm is needed for -lscrypt
LIBS = \
	`pkg-config --cflags --libs gtk+-3.0` \
	-lscrypt -lm \
	-lpwquality \
	-lsqlite3

all:
	gcc -g -o simplesale $(CFILES) -Wall -Wextra -pedantic --std=c99 $(LIBS)
