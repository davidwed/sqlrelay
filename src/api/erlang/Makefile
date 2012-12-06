top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(ERLANGINCLUDES) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c/include -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(ERLANGLIBS) -L$(top_builddir)/src/api/c/src -L$(top_builddir)/src/api/c++/src -lsqlrclientwrapper -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
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

all: sqlrelay.beam sqlrelay

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlrelay.erl
	$(LIBTOOL) --mode=clean $(RM) sqlrelay
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) *.beam
	$(RMTREE) .libs

sqlrelay.erl: sqlrelay.erl.in
	sed -e "s|@SQLRELAY_PATH@|$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/priv/bin|g" $< > $@

sqlrelay.beam: sqlrelay.erl
	$(ERLC) $(ERLCFLAGS) -W $<

sqlrelay: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/src
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/ebin
	$(MKINSTALLDIRS) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/priv/bin
	$(INSTALL) -m 644 sqlrelay.erl $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/src
	$(INSTALL) -m 644 sqlrelay.beam $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/ebin
	$(LTINSTALL) sqlrelay $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)/priv/bin

uninstall:
	$(RMTREE) $(DESTDIR)$(ERLANG_ROOT_DIR)/lib/sqlrelay-$(SQLR_VERSION)
