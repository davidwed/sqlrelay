top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(MYSQLINCLUDES)
LIBS = $(MYSQLLIBS) -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)
ifneq ($(strip $(MYSQLLIBSPATH)),)
ifeq ($(MYSQLUSERPATH),yes)
	RPATH = -R $(MYSQLLIBSPATH)
endif
endif

SRCS= mysqlconnection.cpp
LOBJS= mysqlconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: sqlrconnection_mysql.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_mysql.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT) $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mysql.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mysql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_mysql.a
	$(RM) $(libexecdir)/sqlrconnection_mysql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_mysql.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_mysql.*
