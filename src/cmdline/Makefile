include ../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) -DSYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(abs_srcdir)/src/util/libsqlrutil.la $(abs_srcdir)/src/api/c++/src/libsqlrclient.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(CLIENTUSERPATH),yes)
	RPATH = -rpath $(libdir)
endif
endif

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

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
	$(abs_srcdir)/mkinstalldirs $(bindir)
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
