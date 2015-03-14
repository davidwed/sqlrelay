top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(ODBCINCLUDES) $(ICONVINCLUDES)
LIBS = $(ODBCLIBS) $(ICONVLIBS) -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)
ifneq ($(strip $(ODBCLIBSPATH)),)
ifeq ($(ODBCUSERPATH),yes)
	RPATH = -R $(ODBCLIBSPATH)
endif
endif
ifneq ($(strip $(ICONVLIBSPATH)),)
ifeq ($(ICONVUSERPATH),yes)
	RPATH += -R $(ICONVLIBSPATH)
endif
endif

SRCS= odbcconnection.cpp
LOBJS= odbcconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: sqlrconnection_odbc.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_odbc.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_odbc.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_odbc.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_odbc.a
	$(RM) $(libexecdir)/sqlrconnection_odbc.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_odbc.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_odbc.*
