include ../../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

all:

install:
	$(INSTALL) -m 644 sqlrelay/sqlrclientwrapper.h $(incdir)/sqlrelay

uninstall:
	$(RM) $(incdir)/sqlrclientwrapper.h
	$(RM) $(incdir)/sqlrelay/sqlrclientwrapper.h
