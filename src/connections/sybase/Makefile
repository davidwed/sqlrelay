include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(SYBASEINCLUDES)
LIBS = $(SYBASELIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(SYBASELIBS) $(abs_srcdir)/src/connection/libsqlrconnection_debug.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifeq ($(strip $(SYBASELIBSPATH)),)
	RPATH =
else
ifeq ($(SYBASEUSERPATH),yes)
	RPATH = -rpath $(SYBASELIBSPATH)
endif
endif

SRCS= sybaseconnection.C main.C
LOBJS= sybaseconnection.lo main.lo
DLOBJS= sybaseconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: sqlr-connection-sybase sqlr-connection-sybase-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-sybase
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-sybase-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-sybase: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(SYBASESTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-sybase-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(SYBASESTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-sybase $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-sybase-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-sybase
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-sybase-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
