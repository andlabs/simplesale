OBJDIR = .objs
OUTFILE = simplesale

CFILES = \
	zresources.c

HFILES = \
	zaddremovemoney.h \
	zemployeeeditor.h \
	zitemeditor.h \
	zlogviewer.h \
	zothersettings.h

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

# TODO make zresources.c depend on the .ui files and the .h files not need separate rules (get type name from .ui file)

zresources.c: gresources.xml $(HFILES)
	glib-compile-resources --target $@ --generate-source $<

zaddremovemoney.h: addremovemoney.ui
	go run tools/ui2cdecl.go $< $@ AddRemoveMoney /simplesale/addremovemoney.ui

zemployeeeditor.h: employeeeditor.ui
	go run tools/ui2cdecl.go $< $@ EmployeeEditor /simplesale/employeeeditor.ui

zitemeditor.h: itemeditor.ui
	go run tools/ui2cdecl.go $< $@ ItemEditor /simplesale/itemeditor.ui

zlogviewer.h: logviewer.ui
	go run tools/ui2cdecl.go $< $@ LogViewer /simplesale/logviewer.ui

zothersettings.h: othersettings.ui
	go run tools/ui2cdecl.go $< $@ OtherSettings /simplesale/othersettings.ui

dirs:
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) z*
