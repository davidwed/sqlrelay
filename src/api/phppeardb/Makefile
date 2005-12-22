top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

all:

clean:

rebuild: clean all

install:
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPPEARDBDIR)
	$(INSTALL) -m 644 sqlrelay.php $(DESTDIR)$(PHPPEARDBDIR)

uninstall:
	$(RM) $(DESTDIR)$(PHPPEARDBDIR)/sqlrelay.php
