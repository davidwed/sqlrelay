include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/connection $(RUDIMENTSINCLUDES) $(MSQLINCLUDES)
LIBS = $(MSQLLIBS) -L$(libdir) -lsqlrconnection -L$(libdir) -lsqlrutil $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(MSQLLIBS) -L$(libdir) -lsqlrconnection_debug -L$(libdir) -lsqlrutil $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(MSQLLIBSPATH)),)
ifeq ($(MSQLUSERPATH),yes)
	RPATH = -rpath $(MSQLLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -rpath $(libdir)
endif
endif

SRCS= msqlconnection.C main.C
LOBJS= msqlconnection.lo main.lo
DLOBJS= msqlconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: sqlr-connection-msql sqlr-connection-msql-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-msql
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-msql-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-connection-msql: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MSQLSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-msql-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(MSQLSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-msql $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-connection-msql-debug $(bindir)
	$(abs_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(abs_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-msql
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-msql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
