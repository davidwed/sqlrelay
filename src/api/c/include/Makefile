top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all install uninstall

all:

install:
	$(INSTALL) -m 644 sqlrelay/sqlrclientwrapper.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 sqlrelay/private/wrapperdll.h $(includedir)/sqlrelay/private

uninstall:
	$(RM) $(includedir)/sqlrclientwrapper.h
	$(RM) $(includedir)/sqlrelay/sqlrclientwrapper.h
	$(RM) $(includedir)/sqlrelay/private/wrapperdll.h
