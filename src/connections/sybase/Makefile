top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(SYBASEINCLUDES)
LIBS = $(SYBASELIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)
ifneq ($(strip $(SYBASELIBSPATH)),)
ifeq ($(SYBASEUSERPATH),yes)
	RPATH = -R $(SYBASELIBSPATH)
endif
endif

SRCS= sybaseconnection.cpp
LOBJS= sybaseconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrconnection_sybase.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrconnection_sybase.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrconnection_sybase.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_sybase.a
	$(RM) $(libexecdir)/sqlrconnection_sybase.la
	$(MODULERENAME) $(libexecdir)/sqlrconnection_sybase.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_sybase.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_sybase*
	$(RM) $(bindir)/sqlr-connection-sybase$(EXE)
