top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all install uninstall

all:

install:
	$(INSTALL) -m 644 sqlrelay/sqlrclientwrapper.h $(includedir)/sqlrelay

uninstall:
	$(RM) $(includedir)/sqlrclientwrapper.h
	$(RM) $(includedir)/sqlrelay/sqlrclientwrapper.h
