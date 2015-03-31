top_builddir = ../../..

include ../../../config.mk

SRCS= routerconnection.cpp
LOBJS= routerconnection.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ROUTERCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(ROUTERCONNECTIONCPPFLAGS) $(COMPILE) $<

all: sqlrconnection_router.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_router.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(ROUTERCONNECTIONLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_router.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_router.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_router.a
	$(RM) $(libexecdir)/sqlrconnection_router.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_router.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_router.*
