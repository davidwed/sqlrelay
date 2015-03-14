top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(MDBTOOLSINCLUDES)
LIBS = $(MDBTOOLSLIBS) -L$(top_builddir)/src/server/src -lsqlrserver $(SERVERLIB) -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)
ifneq ($(strip $(MDBTOOLSLIBSPATH)),)
ifeq ($(MDBTOOLSUSERPATH),yes)
	RPATH = -R $(MDBTOOLSLIBSPATH)
endif
endif

SRCS= mdbtoolsconnection.cpp
LOBJS= mdbtoolsconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: sqlrconnection_mdbtools.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_mdbtools.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mdbtools.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mdbtools.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.a
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_mdbtools.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.*
