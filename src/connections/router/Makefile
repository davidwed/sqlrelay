top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)
ifneq ($(strip $(ROUTERLIBSPATH)),)
ifeq ($(ROUTERUSERPATH),yes)
	RPATH = -R $(ROUTERLIBSPATH)
endif
endif

SRCS= routerconnection.cpp
LOBJS= routerconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: sqlrconnection_router.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_router.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_router.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_router.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_router.a
	$(RM) $(libexecdir)/sqlrconnection_router.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_router.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_router.*
