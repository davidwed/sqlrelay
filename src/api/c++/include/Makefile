top_builddir = ../../../..

include ../../../../config.mk

all:

install:
	$(MKINSTALLDIRS) $(incdir)/sqlrelay
	$(MKINSTALLDIRS) $(incdir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/sqlrclient.h $(incdir)/sqlrelay
	$(INSTALL) -m 644 sqlrelay/private/bindvar.h $(incdir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/column.h $(incdir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/row.h $(incdir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrconnection.h $(incdir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrdefines.h $(incdir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrincludes.h $(incdir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrcursor.h $(incdir)/sqlrelay/private

uninstall:
	$(RM) $(incdir)/sqlrclient.h
	$(RM) $(incdir)/private/bindvar.h
	$(RM) $(incdir)/private/column.h
	$(RM) $(incdir)/private/field.h
	$(RM) $(incdir)/private/row.h
	$(RM) $(incdir)/private/sqlrconnection.h
	$(RM) $(incdir)/private/sqlrdefines.h
	$(RM) $(incdir)/private/sqlrincludes.h
	$(RM) $(incdir)/private/sqlrcursor.h
	$(RMTREE) $(incdir)/sqlrelay
