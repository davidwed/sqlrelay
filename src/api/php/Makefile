include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES)
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(PTHREADLIBS) $(STDCPPLIB)
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
	$(MAKE) sql_relay.la 2> errors1; \
	if ( test ! -r sql_relay.la ); then \
		cp phpincludes2.h phpincludes.h; \
		$(MAKE) sql_relay.la 2> errors2; \
	fi
	if ( test ! -r sql_relay.la ); then \
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
	$(LIBTOOL) --mode=clean $(RM) phpincludes.h
	$(LIBTOOL) --mode=clean $(RM) errors1
	$(LIBTOOL) --mode=clean $(RM) errors2

rebuild: clean all

libsql_relay.la: sql_relay.C sql_relay.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sql_relay.lo $(LIBS) -rpath $(PHPEXTDIR) $(RPATH) -module

install:
	cp .libs/libsql_relay.so sql_relay.so
	$(abs_srcdir)/mkinstalldirs $(PHPEXTDIR)
	$(LIBTOOL) --mode=install $(INSTALL) -s sql_relay.so $(PHPEXTDIR)
	$(RM) sql_relay.so

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(PHPEXTDIR)/sql_relay.so
