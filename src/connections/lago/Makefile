include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(LAGOINCLUDES)
LIBS = $(LAGOLIBS) $(abs_srcdir)/src/connection/libsqlrconnection.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(NSLLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(LAGOLIBS) $(abs_srcdir)/src/connection/libsqlrconnection_debug.la $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(NSLLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(LAGOLIBSPATH)),)
ifeq ($(LAGOUSERPATH),yes)
	RPATH = -rpath $(LAGOLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -rpath $(libdir)
endif
endif

SRCS= lagoconnection.C main.C
LOBJS= lagoconnection.lo main.lo
DLOBJS= lagoconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: sqlr-connection-lago sqlr-connection-lago-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-lago
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-lago-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-lago: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(LAGOSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-lago-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(LAGOSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-lago $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-lago-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-lago
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-lago-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
