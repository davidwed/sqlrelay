top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

all:

clean:

rebuild: clean all

install:
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPPEARDBDIR)
	$(INSTALL) sqlrelay.php $(DESTDIR)$(PHPPEARDBDIR)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(DESTDIR)$(PHPPEARDBDIR)/sqlrelay.php
