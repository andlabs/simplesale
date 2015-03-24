OBJDIR = .objs
OUTFILE = simplesale

CFILES = \
	manager.c \
	zresources.c

UIFILES = \
	addremovemoney.ui \
	employeeeditor.ui \
	itemeditor.ui \
	logviewer.ui \
	manager.ui \
	othersettings.ui

HFILES = \
	simplesale.h \
	$(UIFILES:%.ui=z%.h)
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

z%.h: %.ui
	go run tools/ui2cdecl.go $< $@ /simplesale/$<

dirs:
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) z*
