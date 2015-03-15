top_builddir = ../../../..

include ../../../../config.mk

SRCS = sqlrconnection.cpp sqlrcursor.cpp row.cpp
LOBJS = sqlrconnection.lo sqlrcursor.lo row.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPCPPFLAGS) $(COMPILE) $<

all: libsqlrclient.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

libsqlrclient.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LINKFLAGS) $(CPPLIBS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(cachedir)
	$(CHMOD) 777 $(cachedir)

installdll::
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrclient.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrclient.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrclient.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RMTREE) $(cachedir)

uninstalldll:
	$(RM) $(bindir)/libsqlrclient.$(LIBEXT) \
		$(libdir)/libsqlrclient.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/libsqlrclient.$(LIBEXT)
