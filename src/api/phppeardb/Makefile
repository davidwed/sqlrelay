top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

all:
	if ( test "$(PHPMAJORVERSION)" -lt "5" ); \
	then \
		sed -e "s|@LOADEXTENSION@|DB::assertExtension|" sqlrelay.php.in > sqlrelay.php; \
	else \
		sed -e "s|@LOADEXTENSION@|PEAR::loadExtension|" sqlrelay.php.in > sqlrelay.php; \
	fi

clean:
	rm -f sqlrelay.php

rebuild: clean all

install:
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPPEARDBDIR)
	$(INSTALL) sqlrelay.php $(DESTDIR)$(PHPPEARDBDIR)

uninstall:
	$(RM) $(DESTDIR)$(PHPPEARDBDIR)/sqlrelay.php
