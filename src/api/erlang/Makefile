top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(ERLANGINCLUDES) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(ERLANGLIBS) -L$(top_builddir)/src/api/c++/src -lsqlrclientwrapper -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(ERLANGUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = erl_comm.c relay_driver.c
LOBJS = erl_comm.lo relay_driver.lo
 
.SUFFIXES: .lo .erl .beam

.c.lo:
	$(LIBTOOL) --mode=compile $(CC) $(CCFLAGS) $(CPPFLAGS) -c $< -o $@

.erl.beam:
	erlc -W $<

all: sqlrelay.beam libsqlrelay_drv.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) *.so
	$(LIBTOOL) --mode=clean $(RM) *.beam
	$(RMTREE) .libs

rebuild: clean all

libsqlrelay_drv.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CC) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(ERLANG_INSTALL_LIB_DIR)/sqlrelay-$(SQLR_VERSION)/priv/lib $(RPATH) -module -no-undefined

sqlrelay_drv.so:
	if ( test -r .libs/libsqlrelay_drv.0.0.0 ); \
	then \
		cp .libs/libsqlrelay_drv.0.0.0 sqlrelay_drv.so; \
	fi
	if ( test -r .libs/libsqlrelay_drv.0.0 ); \
	then \
		cp .libs/libsqlrelay_drv.0.0 sqlrelay_drv.so; \
	fi
	if ( test -r .libs/libsqlrelay_drv.0 ); \
	then \
		cp .libs/libsqlrelay_drv.0 sqlrelay_drv.so; \
	fi
	if ( test -r .libs/libsqlrelay_drv ); \
	then \
		cp .libs/libsqlrelay_drv sqlrelay_drv.so; \
	fi
	if ( test -r .libs/libsqlrelay_drv.so.0.0.0 ); \
	then \
		cp .libs/libsqlrelay_drv.so.0.0.0 sqlrelay_drv.so; \
	fi
	if ( test -r .libs/libsqlrelay_drv.so.0.0 ); \
	then \
		cp .libs/libsqlrelay_drv.so.0.0 sqlrelay_drv.so; \
	fi
	if ( test -r .libs/libsqlrelay_drv.so.0 ); \
	then \
		cp .libs/libsqlrelay_drv.so.0 sqlrelay_drv.so; \
	fi
	if ( test -r .libs/libsqlrelay_drv.so ); \
	then \
		cp .libs/libsqlrelay_drv.so sqlrelay_drv.so; \
	fi

sqlrelay_drv.dll:
	if ( test -r .libs/cygsqlrelay_drv-0.dll ); \
	then \
		cp .libs/cygsqlrelay_drv-0.dll sqlrelay_drv.dll; \
	fi

ifeq ($(CYGWIN),)
install: sqlrelay_drv.so
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/src
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/ebin
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/priv/lib
	$(INSTALL) -m 644 sqlrelay.erl $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/src
	$(INSTALL) -m 644 sqlrelay.beam $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/ebin
	$(LTINSTALL) sqlrelay_drv.so $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/priv/lib
else
install: sqlrelay_drv.dll
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/src
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/ebin
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/priv/lib
	$(INSTALL) -m 644 sqlrelay.erl $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/src
	$(INSTALL) -m 644 sqlrelay.beam $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/ebin
	$(LTINSTALL) sqlrelay_drv.dll $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/priv/lib
endif

uninstall:
	$(RMTREE) $(DESTDIR)$(ERLANG_INSTALL_LIB_DIR)/sqlrelay-$(SQLR_VERSION)/src
