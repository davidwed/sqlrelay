top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES)
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PHPUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPILE_DL=1 -c $< -o $@

all:
	cp phpincludes1.h phpincludes.h; \
	$(MAKE) libsql_relay.la 2> errors1; \
	if ( test ! -r libsql_relay.la ); then \
		cp phpincludes2.h phpincludes.h; \
		$(MAKE) libsql_relay.la 2> errors2; \
		if ( test ! -r libsql_relay.la ); then \
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
	$(RM) errors1
	$(RM) errors2

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) *.so
	$(LIBTOOL) --mode=clean $(RM) phpincludes.h
	$(LIBTOOL) --mode=clean $(RM) errors1
	$(LIBTOOL) --mode=clean $(RM) errors2
	$(RMTREE) .libs

rebuild: clean all

libsql_relay.la: sql_relay.C sql_relay.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sql_relay.lo $(LDFLAGS) $(LIBS) -rpath $(PHPEXTDIR) $(RPATH) -module -no-undefined

sql_relay.so:
	if ( test -r .libs/libsql_relay.0.0.0 ); \
	then \
		cp .libs/libsql_relay.0.0.0 sql_relay.so; \
	fi
	if ( test -r .libs/libsql_relay.0.0 ); \
	then \
		cp .libs/libsql_relay.0.0 sql_relay.so; \
	fi
	if ( test -r .libs/libsql_relay.0 ); \
	then \
		cp .libs/libsql_relay.0 sql_relay.so; \
	fi
	if ( test -r .libs/libsql_relay ); \
	then \
		cp .libs/libsql_relay sql_relay.so; \
	fi
	if ( test -r .libs/libsql_relay.so.0.0.0 ); \
	then \
		cp .libs/libsql_relay.so.0.0.0 sql_relay.so; \
	fi
	if ( test -r .libs/libsql_relay.so.0.0 ); \
	then \
		cp .libs/libsql_relay.so.0.0 sql_relay.so; \
	fi
	if ( test -r .libs/libsql_relay.so.0 ); \
	then \
		cp .libs/libsql_relay.so.0 sql_relay.so; \
	fi
	if ( test -r .libs/libsql_relay.so ); \
	then \
		cp .libs/libsql_relay.so sql_relay.so; \
	fi

install: sql_relay.so
	$(MKINSTALLDIRS) $(DESTDIR)$(PHPEXTDIR)
	$(INSTALL) sql_relay.so $(DESTDIR)$(PHPEXTDIR)
	$(RM) sql_relay.so

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(DESTDIR)$(PHPEXTDIR)/sql_relay.so
