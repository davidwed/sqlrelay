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

SRCS = sqlrelayCmd.C
LOBJS = sqlrelayCmd.lo

all:
	cp tclincludes1.h tclincludes.h; \
	$(MAKE) libsqlrelay.la 2> errors1; \
	if ( test ! -r libsqlrelay.la ); then \
		cp tclincludes2.h tclincludes.h; \
		$(MAKE) libsqlrelay.la 2> errors2; \
		if ( test ! -r libsqlrelay.la ); then \
			if ( test -s errors1 ); then \
				echo "first try errors : "; cat errors1; echo; \
			fi; \
			if ( test -s errors2 ); then \
				echo "second try errors : "; cat errors2; echo; \
			fi; \
			exit 1; \
		else \
			cat errors2; \
		fi; \
	else \
		cat errors1; \
	fi
	$(RM) errors1 errors2 tclincludes.h

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o	
	$(LIBTOOL) --mode=clean $(RM) *.so
	$(LIBTOOL) --mode=clean $(RM) tclincludes.h
	$(LIBTOOL) --mode=clean $(RM) errors1
	$(LIBTOOL) --mode=clean $(RM) errors2

rebuild: clean all

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

libsqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) -rpath $(TCLLIBSPATH)/sqlrelay $(RPATH) -module -no-undefined $(LIBS)

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
	$(INSTALL) $(INSTALLSTRIP) sqlrelay.so $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(INSTALL) -m 644 pkgIndex.tcl $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(RM) sqlrelay.so
else
install: sqlrelay.dll
	$(MKINSTALLDIRS) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(INSTALL) $(INSTALLSTRIP) sqlrelay.dll $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(INSTALL) -m 644 pkgIndex.tcl $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(RM) sqlrelay.dll
endif

uninstall:
	$(RMTREE) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
