top_builddir = ../../..

include ../../../config.mk

SRCS= sqliteconnection.cpp
LOBJS= sqliteconnection.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SQLITECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SQLITECONNECTIONCPPFLAGS) $(COMPILE) $<

all: sqlrconnection_sqlite.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_sqlite.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(SQLITECONNECTIONLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sqlite.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sqlite.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_sqlite.a
	$(RM) $(libexecdir)/sqlrconnection_sqlite.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_sqlite.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_sqlite.*
