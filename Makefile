CFILES = accounteditor.c \
	accounts.c \
	db.c \
	items.c \
	itemeditor.c \
	language.c \
	login.c \
	main.c \
	manager.c \
	order.c \
	othersettings.c \
	ow-crypt.c \
	passentry.c \
	paydialog.c \
	price.c \
	shift.c \
	util.c

HFILES = dbschema.h simplesale.h

ALLFILES = $(CFILES) $(HFILES)

LIBS = \
	`pkg-config --cflags --libs gtk+-3.0` \
	-lpwquality \
	-lsqlite3

all:
	gcc -g -o simplesale $(CFILES) -Wall -Wextra -pedantic --std=c99 $(LIBS)
