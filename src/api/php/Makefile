include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES)
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(STDCPPLIB)
RPATH = -rpath /lib
ifneq ($(strip $(libdir)),)
ifeq ($(PHPUSERPATH),yes)
	RPATH = -rpath $(libdir)
endif
endif

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPILE_DL=1 -c $< -o $@

all:
	cp phpincludes1.h phpincludes.h; \
	$(MAKE) sql_relay.so 2> errors1; \
	if ( test ! -r sql_relay.so ); then \
		cp phpincludes2.h phpincludes.h; \
		$(MAKE) sql_relay.so 2> errors2; \
	fi
	if ( test ! -r sql_relay.so ); then \
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
	$(LIBTOOL) --mode=clean $(RM) phpincludes.h
	$(LIBTOOL) --mode=clean $(RM) errors1
	$(LIBTOOL) --mode=clean $(RM) errors2
	$(LIBTOOL) --mode=clean $(RM) sql_relay.so

rebuild: clean all

sql_relay.so: libsql_relay.la
	for i in `find .libs -name "libsql_relay*so*"`; \
	do \
		cp $$i sql_relay.so; \
	done
	$(LIBTOOL) --mode=clean $(RM) *.la

libsql_relay.la: sql_relay.C sql_relay.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sql_relay.lo $(LIBS) $(RPATH) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(PHPEXTDIR)
	$(INSTALL) -s sql_relay.so $(PHPEXTDIR)

uninstall:
	$(RM) $(PHPEXTDIR)/sql_relay.so
