top_builddir = ../..

include ../../config.mk

INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) -DSYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/util -lsqlrutil -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(CLIENTUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

all: query fields sqlrsh

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) fields
	$(LIBTOOL) --mode=clean $(RM) fields-debug
	$(LIBTOOL) --mode=clean $(RM) query
	$(LIBTOOL) --mode=clean $(RM) query-debug
	$(LIBTOOL) --mode=clean $(RM) sqlrsh
	$(LIBTOOL) --mode=clean $(RM) sqlrsh-debug

rebuild: clean all

query: query.C query.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ query.lo $(LIBS) $(RPATH)

fields: fields.C fields.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ fields.lo $(LIBS) $(RPATH)

sqlrsh: sqlrsh.C sqlrsh.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.lo $(LIBS) $(READLINELIB) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) query $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) fields $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlrsh $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/query
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/query-debug
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/fields
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/fields-debug
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlrsh
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlrsh-debug
