top_builddir = ../../..

include ../../../config.mk

SRCS = sqlrclientwrapper.cpp
LOBJS = sqlrclientwrapper.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CCPPFLAGS) $(COMPILE) $<

all: libsqlrclientwrapper.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

libsqlrclientwrapper.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(CLIBS) $(LINKFLAGS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(cachedir)
	$(CHMOD) 777 $(cachedir)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(CP) sqlrelay/sqlrclientwrapper.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrclientwrapper.h
	$(CP) sqlrelay/private/sqlrclientwrapper.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrclientwrapper.h
	$(CP) sqlrelay/private/wrapperdll.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/wrapperdll.h

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrclientwrapper.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrclientwrapper.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrclientwrapper.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RMTREE) $(cachedir)
	$(RM) $(includedir)/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/private/sqlrclientwrapper.h \
		$(includedir)/sqlrelay/private/wrapperdll.h

uninstalldll:
	$(RM) $(bindir)/libsqlrclientwrapper.$(LIBEXT) \
		$(libdir)/libsqlrclientwrapper.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/libsqlrclientwrapper.$(LIBEXT)
