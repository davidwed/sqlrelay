top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/authenticator -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(FREETDSINCLUDES)
LIBS = $(FREETDSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(FREETDSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection_debug -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(FREETDSLIBSPATH)),)
ifeq ($(FREETDSUSERPATH),yes)
	RPATH = -R $(FREETDSLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= freetdsconnection.C main.C
LOBJS= freetdsconnection.lo main.lo
DLOBJS= freetdsconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	if ( test "$(STRIP)" != "touch" ); then \
		$(STRIP) -x -R .note -R .comment $@; \
	fi

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@
	if ( test "$(STRIP)" != "touch" ); then \
		$(STRIP) -x -R .note -R .comment $@; \
	fi

all: sqlr-connection-freetds sqlr-connection-freetds-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-freetds
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-freetds-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-freetds: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(FREETDSSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-freetds-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(FREETDSSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-freetds $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-freetds-debug $(bindir)
	$(MKINSTALLDIRS) $(tmpdir)
	chmod 777 $(tmpdir)
	$(MKINSTALLDIRS) $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-freetds
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-freetds-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
