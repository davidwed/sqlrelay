include ../../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/api/c++/include -I$(abs_srcdir)/src/common $(RUDIMENTSINCLUDES)

SRCS = connection/constructor.C \
	connection/destructor.C \
	connection/ping.C \
	connection/identify.C \
	connection/endsession.C \
	connection/suspendsession.C \
	connection/resumesession.C \
	connection/autocommit.C \
	connection/commit.C \
	connection/rollback.C \
	connection/debug.C \
	connection/error.C \
	connection/getreconnect.C \
	connection/getconnectionport.C \
	connection/getconnectionsocket.C \
	connection/authentication.C \
	connection/getnewport.C \
	connection/copyreferences.C \
	connection/clearsessionflags.C \
	connection/opensession.C \
	connection/closeconnection.C \
	cursor/sqlrcursor.C \
	row/row.C
NLOBJS = connection/constructor.n.lo \
	connection/destructor.n.lo \
	connection/ping.n.lo \
	connection/identify.n.lo \
	connection/endsession.n.lo \
	connection/suspendsession.n.lo \
	connection/resumesession.n.lo \
	connection/autocommit.n.lo \
	connection/commit.n.lo \
	connection/rollback.n.lo \
	connection/debug.n.lo \
	connection/error.n.lo \
	connection/getreconnect.n.lo \
	connection/getconnectionport.n.lo \
	connection/getconnectionsocket.n.lo \
	connection/authentication.n.lo \
	connection/getnewport.n.lo \
	connection/copyreferences.n.lo \
	connection/clearsessionflags.n.lo \
	connection/opensession.n.lo \
	connection/closeconnection.n.lo \
	cursor/sqlrcursor.n.lo \
	row/row.n.lo
PLOBJS = connection/constructor.p.lo \
	connection/destructor.p.lo \
	connection/ping.p.lo \
	connection/identify.p.lo \
	connection/endsession.p.lo \
	connection/suspendsession.p.lo \
	connection/resumesession.p.lo \
	connection/autocommit.p.lo \
	connection/commit.p.lo \
	connection/rollback.p.lo \
	connection/debug.p.lo \
	connection/error.p.lo \
	connection/getreconnect.p.lo \
	connection/getconnectionport.p.lo \
	connection/getconnectionsocket.p.lo \
	connection/authentication.p.lo \
	connection/getnewport.p.lo \
	connection/copyreferences.p.lo \
	connection/clearsessionflags.p.lo \
	connection/opensession.p.lo \
	connection/closeconnection.p.lo \
	cursor/sqlrcursor.p.lo \
	row/row.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrclient.la libsqlrclient_p.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) */*.lo
	$(LIBTOOL) --mode=clean $(RM) */*.o

rebuild: clean all

libsqlrclient.la: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(NLOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libsqlrclient_p.la: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(PLOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -s libsqlrclient.la $(libdir)/libsqlrclient.la
	$(LIBTOOL) --mode=install $(INSTALL) libsqlrclient_p.la $(libdir)/libsqlrclient_p.la
	$(LIBTOOL) --mode=finish $(libdir)
	$(abs_srcdir)/mkinstalldirs $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclient.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclient_p.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
