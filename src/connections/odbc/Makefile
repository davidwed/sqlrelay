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
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrconnection_odbc.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrconnection_odbc.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_odbc.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_odbc.a
	$(RM) $(libexecdir)/sqlrconnection_odbc.la
	$(MODULERENAME) $(libexecdir)/sqlrconnection_odbc.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_odbc.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_odbc*
	$(RM) $(bindir)/sqlr-connection-odbc$(EXE)
