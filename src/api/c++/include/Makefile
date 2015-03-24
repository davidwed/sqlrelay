top_builddir = ../../../..

include ../../../../config.mk

all:

install:
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(CP) sqlrelay/sqlrclient.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrclient.h
	$(CP) sqlrelay/private/bindvar.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/bindvar.h
	$(CP) sqlrelay/private/column.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/column.h
	$(CP) sqlrelay/private/row.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/row.h
	$(CP) sqlrelay/private/listformat.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/listformat.h
	$(CP) sqlrelay/private/sqlrconnection.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrconnection.h
	$(CP) sqlrelay/private/sqlrdefines.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrdefines.h
	$(CP) sqlrelay/private/sqlrincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrincludes.h
	$(CP) sqlrelay/private/sqlrcursor.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrcursor.h
	$(CP) sqlrelay/private/dll.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/dll.h

uninstall:
	$(RM) $(includedir)/sqlrclient.h \
		$(includedir)/private/bindvar.h \
		$(includedir)/private/column.h \
		$(includedir)/private/field.h \
		$(includedir)/private/row.h \
		$(includedir)/private/listformat.h \
		$(includedir)/private/sqlrconnection.h \
		$(includedir)/private/sqlrdefines.h \
		$(includedir)/private/sqlrincludes.h \
		$(includedir)/private/sqlrcursor.h
	$(RMTREE) $(includedir)/sqlrelay
