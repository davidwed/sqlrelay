top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/authenticator -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(MDBTOOLSINCLUDES) $(SIDINCLUDES)
LIBS = $(MDBTOOLSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
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

all: libsqlrelay_sqlrconnection_mdbtools.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

libsqlrelay_sqlrconnection_mdbtools.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrelay_sqlrconnection_mdbtools.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_mdbtools.a

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqlrconnection_mdbtools.la
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-mdbtools$(EXE)
