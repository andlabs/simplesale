# TODO
# - remove $(OUTFILE) in make clean?

OBJDIR = .objs
OUTFILE = simplesale

IFACEFILES = \
	backend.iface

CFILES = \
	addremovemoney.c \
	deviceeditor.c \
	employeeeditor.c \
	itemeditor.c \
	logviewer.c \
	manager.c \
	mockbackend.c \
	othersettings.c \
	price.c \
	priceentry.c \
	zresources.c \
	$(IFACEFILES:%.iface=z%.c)
# see comment below
.PRECIOUS: $(CFILES)

UIFILES = \
	addremovemoney.ui \
	deviceeditor.ui \
	employeeeditor.ui \
	itemeditor.ui \
	logviewer.ui \
	manager.ui \
	othersettings.ui

HFILES = \
	simplesale.h \
	$(UIFILES:%.ui=z%.h) \
	$(IFACEFILES:%.iface=z%.h)
# see http://stackoverflow.com/a/29223637/3408572
.PRECIOUS: $(HFILES)

OFILES = $(CFILES:%.c=$(OBJDIR)/%.o)
PKGS = gtk+-3.0

xCFLAGS = \
	--std=c99 \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	`pkg-config --cflags $(PKGS)` \
	$(CFLAGS)

xLDFLAGS = \
	`pkg-config --libs $(PKGS)` \
	$(LDFLAGS)

$(OUTFILE): $(OFILES)
	$(CC) -g -o $(OUTFILE) $(OFILES) $(xLDFLAGS)

$(OBJDIR)/%.o: %.c $(HFILES) dirs
	$(CC) -g -o $@ -c $< $(xCFLAGS)

zresources.c: gresources.xml $(UIFILES)
	glib-compile-resources --target $@ --generate-source $<

z%.c: %.iface
	go run tools/iface2cdecl.go $< cfile $@

z%.h: %.ui
	go run tools/ui2cdecl.go $< $@ /simplesale/$<

z%.h: %.iface
	go run tools/iface2cdecl.go $< hfile $@

dirs:
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) z*
