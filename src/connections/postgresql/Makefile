top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(POSTGRESQLINCLUDES)
LIBS = $(POSTGRESQLLIBS) -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)
ifneq ($(strip $(POSTGRESQLLIBSPATH)),)
ifeq ($(POSTGRESQLUSERPATH),yes)
	RPATH = -R $(POSTGRESQLLIBSPATH)
endif
endif

SRCS= postgresqlconnection.cpp
LOBJS= postgresqlconnection.lo

.SUFFIXES: .lo .d.lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: sqlrconnection_postgresql.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_postgresql.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_postgresql.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_postgresql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_postgresql.a
	$(RM) $(libexecdir)/sqlrconnection_postgresql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_postgresql.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_postgresql.*
