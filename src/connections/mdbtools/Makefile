top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/authenticator -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(MDBTOOLSINCLUDES)
LIBS = $(MDBTOOLSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)
ifneq ($(strip $(MDBTOOLSLIBSPATH)),)
ifeq ($(MDBTOOLSUSERPATH),yes)
	RPATH = -R $(MDBTOOLSLIBSPATH)
endif
endif

SRCS= mdbtoolsconnection.cpp
LOBJS= mdbtoolsconnection.lo

.SUFFIXES: .lo .d.lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrconnection_mdbtools.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrconnection_mdbtools.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrconnection_mdbtools.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.a
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.la
	$(MODULERENAME) $(libexecdir)/sqlrconnection_mdbtools.$(SOSUFFIX) $(SOSUFFIX) $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_mdbtools*
	$(RM) $(bindir)/sqlr-connection-mdbtools$(EXE)
