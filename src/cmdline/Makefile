include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) -DSYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" -I./ -I../../ -I../common -I../util -I../api/c++/include $(RUDIMENTSINCLUDES)
CONFIGFILELIBS = -L../util -lsqlrutil
LIBS = -L../api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: query query-debug fields fields-debug sqlrsh sqlrsh-debug

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
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ query.lo $(CONFIGFILELIBS) $(LIBS)

query-debug: query.C query.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ query.p.lo $(CONFIGFILELIBS) $(LIBS)

fields: fields.C fields.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ fields.lo $(CONFIGFILELIBS) $(LIBS)

fields-debug: fields.C fields.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ fields.p.lo $(CONFIGFILELIBS) $(LIBS)

sqlrsh: sqlrsh.C sqlrsh.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.lo $(CONFIGFILELIBS) $(LIBS) $(READLINELIB)

sqlrsh-debug: sqlrsh.C sqlrsh.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.lo $(CONFIGFILELIBS) $(LIBS) $(READLINELIB)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s query $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) query-debug $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s fields $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) fields-debug $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlrsh $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlrsh-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/query
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/query-debug
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/fields
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/fields-debug
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlrsh
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlrsh-debug
