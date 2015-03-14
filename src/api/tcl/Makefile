top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(TCLINCLUDE) -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(TCLLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(TCLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = sqlrelayCmd.cpp
LOBJS = sqlrelayCmd.lo

all: sqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o *.so
	$(RMTREE) .libs

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

sqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(TCLLIBSPATH)/sqlrelay $(RPATH) -avoid-version -module -no-undefined

install: sqlrelay.la
	$(MKINSTALLDIRS) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(LTINSTALL) $(CP) sqlrelay.la $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(RM) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay/sqlrelay.la
	$(RM) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay/sqlrelay.a
	$(INSTALL) -m 644 pkgIndex.tcl $(DESTDIR)$(TCLLIBSPATH)/sqlrelay

uninstall:
	$(RMTREE) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
