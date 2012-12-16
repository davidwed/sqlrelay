top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(ODBCINCLUDES) $(ICONVINCLUDES)
LIBS = $(ODBCLIBS) $(ICONVLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS)
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
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrconnection_odbc.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_odbc.a
	$(RM) $(libexecdir)/sqlrconnection_odbc.la
	$(MODULERENAME) $(libexecdir)/sqlrconnection_odbc.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_odbc.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_odbc*
	$(RM) $(bindir)/sqlr-connection-odbc$(EXE)
