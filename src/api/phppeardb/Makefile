top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

all:

clean:

install:
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPPEARDBDIR)
	$(INSTALL) -m 644 sqlrelay.php $(DESTDIR)$(PHPPEARDBDIR)

uninstall:
	$(RM) $(DESTDIR)$(PHPPEARDBDIR)/sqlrelay.php
