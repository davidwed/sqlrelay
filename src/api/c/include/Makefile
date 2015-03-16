top_builddir = ../../../../

include ../../../../config.mk

all:

install:
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(CP) sqlrelay/sqlrclientwrapper.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrclientwrapper.h
	$(CP) sqlrelay/private/sqlrclientwrapper.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrclientwrapper.h
	$(CP) sqlrelay/private/wrapperdll.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/wrapperdll.h

uninstall:
	$(RM) $(includedir)/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/private/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/private/wrapperdll.h
