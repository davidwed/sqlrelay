include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(ORACLEINCLUDES)
LIBS = $(ORACLELIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(ORACLELIBS) $(abs_srcdir)/src/connection/libsqlrconnection_debug.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(ORACLELIBSPATH)),)
ifeq ($(ORACLEUSERPATH),yes)
	RPATH = -rpath $(ORACLELIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -rpath $(libdir)
endif
endif

SRCS= oracle7connection.C main.C
LOBJS= oracle7connection.lo main.lo
DLOBJS= oracle7connection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: sqlr-connection-oracle7 sqlr-connection-oracle7-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle7
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle7-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-oracle7: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-oracle7-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-oracle7 $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-oracle7-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle7
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle7-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
