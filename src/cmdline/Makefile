include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) -DSYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" -I./ -I../../ -I../common -I../util -I../api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L../util -lsqlrutil -L../api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = -L../util -lsqlrutil_p -L../api/c++/src -lsqlrclient_p $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)

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
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ query.lo $(LIBS)

query-debug: query.C query.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ query.p.lo $(DEBUGLIBS)

fields: fields.C fields.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ fields.lo $(LIBS)

fields-debug: fields.C fields.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ fields.p.lo $(DEBUGLIBS)

sqlrsh: sqlrsh.C sqlrsh.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.lo $(LIBS) $(READLINELIB)

sqlrsh-debug: sqlrsh.C sqlrsh.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.lo $(DEBUGLIBS) $(READLINELIB)

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
