top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all install uninstall

all:

install:
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/sqlrclient.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 sqlrelay/private/bindvar.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/column.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/row.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrconnection.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrdefines.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrincludes.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrcursor.h $(includedir)/sqlrelay/private

uninstall:
	$(RM) $(includedir)/sqlrclient.h
	$(RM) $(includedir)/private/bindvar.h
	$(RM) $(includedir)/private/column.h
	$(RM) $(includedir)/private/field.h
	$(RM) $(includedir)/private/row.h
	$(RM) $(includedir)/private/sqlrconnection.h
	$(RM) $(includedir)/private/sqlrdefines.h
	$(RM) $(includedir)/private/sqlrincludes.h
	$(RM) $(includedir)/private/sqlrcursor.h
	$(RMTREE) $(includedir)/sqlrelay
