include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(TCLINCLUDE) -I$(abs_srcdir) -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES) -fPIC
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(PTHREADLIBS) $(STDCPPLIB) $(TCLLIBS) $(SSLLIB) $(CRYPTOLIB) $(LIBDMALLOC) $(LIBEFENCE)
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
	fi
	if ( test ! -r libsqlrelay.la ); then \
		if ( test -s errors1 ); then \
			echo "first try errors : "; cat errors1; echo; \
		fi; \
		if ( test -s errors2 ); then \
			echo "second try errors : "; cat errors2; echo; \
		fi; \
	fi
	$(RM) errors1
	$(RM) errors2

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
	if ( test "$(STRIP)" != "touch" ); then \
		$(STRIP) -x -R .note -R .comment $@; \
	fi

libsqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LIBS) -rpath $(TCLLIBSPATH)/sqlrelay $(RPATH) -module

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

install: sqlrelay.so
	$(abs_srcdir)/mkinstalldirs $(TCLLIBSPATH)/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlrelay.so $(TCLLIBSPATH)/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) -m 644 pkgIndex.tcl $(TCLLIBSPATH)/sqlrelay
	$(RM) sqlrelay.so

uninstall:
	$(RMTREE) $(TCLLIBSPATH)/sqlrelay
