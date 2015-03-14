top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(FIREBIRDINCLUDES)
LIBS = $(FIREBIRDLIBS) -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)
ifneq ($(strip $(FIREBIRDLIBSPATH)),)
ifeq ($(FIREBIRDUSERPATH),yes)
	RPATH = -R $(FIREBIRDLIBSPATH)
endif
endif

SRCS= firebirdconnection.cpp
LOBJS= firebirdconnection.lo

.SUFFIXES: .lo .d.lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrconnection_firebird.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrconnection_firebird.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_firebird.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_firebird.a
	$(RM) $(libexecdir)/sqlrconnection_firebird.la
	$(MODULERENAME) $(libexecdir)/sqlrconnection_firebird.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_firebird.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_firebird*
	$(RM) $(bindir)/sqlr-connection-firebird$(EXE)
