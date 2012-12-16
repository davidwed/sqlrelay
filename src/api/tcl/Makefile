top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

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

all: libsqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o *.so
	$(RMTREE) .libs

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

libsqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(TCLLIBSPATH)/sqlrelay $(RPATH) -module -no-undefined

sqlrelay.so:
	if ( test -r .libs/libsqlrelay.0.0.0 ); \
	then \
		cp .libs/libsqlrelay.0.0.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.0.0 ); \
	then \
		cp .libs/libsqlrelay.0.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.0 ); \
	then \
		cp .libs/libsqlrelay.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay ); \
	then \
		cp .libs/libsqlrelay sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.so.0.0.0 ); \
	then \
		cp .libs/libsqlrelay.so.0.0.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.so.0.0 ); \
	then \
		cp .libs/libsqlrelay.so.0.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.so.0 ); \
	then \
		cp .libs/libsqlrelay.so.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.so ); \
	then \
		cp .libs/libsqlrelay.so sqlrelay.so; \
	fi

sqlrelay.dll:
	if ( test -r .libs/cygsqlrelay-0.dll ); \
	then \
		cp .libs/cygsqlrelay-0.dll sqlrelay.dll; \
	fi

ifeq ($(CYGWIN),)
install: sqlrelay.so
	$(MKINSTALLDIRS) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(LTINSTALL) sqlrelay.so $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(INSTALL) -m 644 pkgIndex.tcl $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(RM) sqlrelay.so
else
install: sqlrelay.dll
	$(MKINSTALLDIRS) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(LTINSTALL) sqlrelay.dll $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(INSTALL) -m 644 pkgIndex.tcl $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(RM) sqlrelay.dll
endif

uninstall:
	$(RMTREE) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
