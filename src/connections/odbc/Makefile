top_builddir = ../../..

include ../../../config.mk

SRCS= odbcconnection.cpp
LOBJS= odbcconnection.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ODBCCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(ODBCCONNECTIONCPPFLAGS) $(COMPILE) $<

all: sqlrconnection_odbc.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_odbc.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(ODBCCONNECTIONLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_odbc.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_odbc.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_odbc.a
	$(RM) $(libexecdir)/sqlrconnection_odbc.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_odbc.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_odbc.*
