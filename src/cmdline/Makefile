include ../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) -DSYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(abs_srcdir)/src/util/libsqlrutil.la $(abs_srcdir)/src/api/c++/src/libsqlrclient.la $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(abs_srcdir)/src/util/libsqlrutil_p.la $(abs_srcdir)/src/api/c++/src/libsqlrclient_p.la $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
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

query: query.C query.n.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ query.n.lo $(LIBS)

query-debug: query.C query.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ query.p.lo $(DEBUGLIBS)

fields: fields.C fields.n.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ fields.n.lo $(LIBS)

fields-debug: fields.C fields.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ fields.p.lo $(DEBUGLIBS)

sqlrsh: sqlrsh.C sqlrsh.n.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.n.lo $(LIBS) $(READLINELIB)

sqlrsh-debug: sqlrsh.C sqlrsh.p.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.p.lo $(DEBUGLIBS) $(READLINELIB)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
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
