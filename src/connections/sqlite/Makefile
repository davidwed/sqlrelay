top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(SQLITEINCLUDES)
LIBS = $(SQLITELIBS) -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS) 
ifneq ($(strip $(SQLITELIBSPATH)),)
ifeq ($(SQLITEUSERPATH),yes)
	RPATH = -R $(SQLITELIBSPATH)
endif
endif

SRCS= sqliteconnection.cpp
LOBJS= sqliteconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

all: sqlrconnection_sqlite.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_sqlite.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sqlite.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sqlite.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_sqlite.a
	$(RM) $(libexecdir)/sqlrconnection_sqlite.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_sqlite.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_sqlite.*
