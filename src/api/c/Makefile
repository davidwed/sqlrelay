top_builddir = ../../..

include ../../../config.mk

SRCS = sqlrclientwrapper.cpp
LOBJS = sqlrclientwrapper.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CCPPFLAGS) $(COMPILE) $<

all: lib$(SQLR)clientwrapper.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

lib$(SQLR)clientwrapper.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(CLIBS) $(CLTLINKFLAGS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(CP) sqlrelay/sqlrclientwrapper.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrclientwrapper.h
	$(CP) sqlrelay/private/sqlrclientwrapperincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrclientwrapperincludes.h
	$(CP) sqlrelay/private/sqlrclientwrapper.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrclientwrapper.h

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) lib$(SQLR)clientwrapper.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) lib$(SQLR)clientwrapper.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) lib$(SQLR)clientwrapper.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RM) $(includedir)/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/private/sqlrclientwrapperincludes.h \
		$(includedir)/sqlrelay/private/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/private/sqlrclientwrapperdll.h \
		$(includedir)/sqlrelay/private/wrapperdll.h

uninstalldll:
	$(RM) $(bindir)/lib$(SQLR)clientwrapper.$(LIBEXT) \
		$(libdir)/lib$(SQLR)clientwrapper.lib \
		$(bindir)/libsqlrclientwrapper.$(LIBEXT) \
		$(libdir)/libsqlrclientwrapper.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/lib$(SQLR)clientwrapper.$(LIBEXT) \
				 $(libdir)/libsqlrclientwrapper.$(LIBEXT)
