top_builddir = ../..

include ../../config.mk

.PHONY: all clean rebuild install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(SIDINCLUDES)
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = sqlrconnection/sqlrconnectionctor.C \
	sqlrconnection/sqlrconnectiondtor.C \
	sqlrconnection/flushwritebuffer.C \
	sqlrconnection/initconnection.C \
	sqlrconnection/ipc.C \
	sqlrconnection/opensockets.C \
	sqlrconnection/listen.C \
	sqlrconnection/listenercomm.C \
	sqlrconnection/clientsession.C \
	sqlrconnection/authenticate.C \
	sqlrconnection/suspendsession.C \
	sqlrconnection/endsession.C \
	sqlrconnection/ping.C \
	sqlrconnection/identify.C \
	sqlrconnection/autocommit.C \
	sqlrconnection/commit.C \
	sqlrconnection/connectioncount.C \
	sqlrconnection/rollback.C \
	sqlrconnection/newquery.C \
	sqlrconnection/handlequery.C \
	sqlrconnection/handleerror.C \
	sqlrconnection/getbinds.C \
	sqlrconnection/getsendcolumninfo.C \
	sqlrconnection/reexecutequery.C \
	sqlrconnection/fetchfrombindcursor.C \
	sqlrconnection/fetchresultset.C \
	sqlrconnection/returnresultsetheader.C \
	sqlrconnection/sendrowcounts.C \
	sqlrconnection/returnoutputbindvalues.C \
	sqlrconnection/sendcolumns.C \
	sqlrconnection/returnresultsetdata.C \
	sqlrconnection/skiprows.C \
	sqlrconnection/sendfield.C \
	sqlrconnection/abortresultset.C \
	sqlrconnection/suspendresultset.C \
	sqlrconnection/resumeresultset.C \
	sqlrconnection/closeconnection.C \
	sqlrconnection/closecursors.C \
	sqlrconnection/temptables.C \
	sqlrconnection/changeuser.C \
	sqlrconnection/relogin.C \
	sqlrconnection/markdatabasedavailable.C \
	sqlrconnection/getuser.C \
	sqlrconnection/getpassword.C \
	sqlrconnection/connectstringvalue.C \
	sqlrconnection/istransactional.C \
	sqlrconnection/bindparameters.C \
	sqlrconnection/unixsocketseqfile.C \
	sqlrcursor/sqlrcursorctor.C \
	sqlrcursor/sqlrcursordtor.C \
	sqlrcursor/openclose.C \
	sqlrcursor/preparequery.C \
	sqlrcursor/queryparse.C \
	sqlrcursor/binds.C \
	sqlrcursor/fakebinds.C \
	sqlrcursor/abort.C \
	sqlrcursor/sid.C
LOBJS = sqlrconnection/sqlrconnectionctor.lo \
	sqlrconnection/sqlrconnectiondtor.lo \
	sqlrconnection/flushwritebuffer.lo \
	sqlrconnection/initconnection.lo \
	sqlrconnection/ipc.lo \
	sqlrconnection/opensockets.lo \
	sqlrconnection/listen.lo \
	sqlrconnection/listenercomm.lo \
	sqlrconnection/clientsession.lo \
	sqlrconnection/authenticate.lo \
	sqlrconnection/suspendsession.lo \
	sqlrconnection/endsession.lo \
	sqlrconnection/ping.lo \
	sqlrconnection/identify.lo \
	sqlrconnection/autocommit.lo \
	sqlrconnection/commit.lo \
	sqlrconnection/connectioncount.lo \
	sqlrconnection/rollback.lo \
	sqlrconnection/newquery.lo \
	sqlrconnection/handlequery.lo \
	sqlrconnection/handleerror.lo \
	sqlrconnection/getbinds.lo \
	sqlrconnection/getsendcolumninfo.lo \
	sqlrconnection/reexecutequery.lo \
	sqlrconnection/fetchfrombindcursor.lo \
	sqlrconnection/fetchresultset.lo \
	sqlrconnection/returnresultsetheader.lo \
	sqlrconnection/sendrowcounts.lo \
	sqlrconnection/returnoutputbindvalues.lo \
	sqlrconnection/sendcolumns.lo \
	sqlrconnection/returnresultsetdata.lo \
	sqlrconnection/skiprows.lo \
	sqlrconnection/sendfield.lo \
	sqlrconnection/abortresultset.lo \
	sqlrconnection/suspendresultset.lo \
	sqlrconnection/resumeresultset.lo \
	sqlrconnection/closeconnection.lo \
	sqlrconnection/closecursors.lo \
	sqlrconnection/temptables.lo \
	sqlrconnection/changeuser.lo \
	sqlrconnection/relogin.lo \
	sqlrconnection/markdatabasedavailable.lo \
	sqlrconnection/getuser.lo \
	sqlrconnection/getpassword.lo \
	sqlrconnection/connectstringvalue.lo \
	sqlrconnection/istransactional.lo \
	sqlrconnection/bindparameters.lo \
	sqlrconnection/unixsocketseqfile.lo \
	sqlrcursor/sqlrcursorctor.lo \
	sqlrcursor/sqlrcursordtor.lo \
	sqlrcursor/openclose.lo \
	sqlrcursor/preparequery.lo \
	sqlrcursor/queryparse.lo \
	sqlrcursor/binds.lo \
	sqlrcursor/fakebinds.lo \
	sqlrcursor/abort.lo \
	sqlrcursor/sid.lo
DLOBJS = sqlrconnection/sqlrconnectionctor.d.lo \
	sqlrconnection/sqlrconnectiondtor.d.lo \
	sqlrconnection/flushwritebuffer.d.lo \
	sqlrconnection/initconnection.d.lo \
	sqlrconnection/ipc.d.lo \
	sqlrconnection/opensockets.d.lo \
	sqlrconnection/listen.d.lo \
	sqlrconnection/listenercomm.d.lo \
	sqlrconnection/clientsession.d.lo \
	sqlrconnection/authenticate.d.lo \
	sqlrconnection/suspendsession.d.lo \
	sqlrconnection/endsession.d.lo \
	sqlrconnection/ping.d.lo \
	sqlrconnection/identify.d.lo \
	sqlrconnection/autocommit.d.lo \
	sqlrconnection/commit.d.lo \
	sqlrconnection/connectioncount.d.lo \
	sqlrconnection/rollback.d.lo \
	sqlrconnection/newquery.d.lo \
	sqlrconnection/handlequery.d.lo \
	sqlrconnection/handleerror.d.lo \
	sqlrconnection/getbinds.d.lo \
	sqlrconnection/getsendcolumninfo.d.lo \
	sqlrconnection/reexecutequery.d.lo \
	sqlrconnection/fetchfrombindcursor.d.lo \
	sqlrconnection/fetchresultset.d.lo \
	sqlrconnection/returnresultsetheader.d.lo \
	sqlrconnection/sendrowcounts.d.lo \
	sqlrconnection/returnoutputbindvalues.d.lo \
	sqlrconnection/sendcolumns.d.lo \
	sqlrconnection/returnresultsetdata.d.lo \
	sqlrconnection/skiprows.d.lo \
	sqlrconnection/sendfield.d.lo \
	sqlrconnection/abortresultset.d.lo \
	sqlrconnection/suspendresultset.d.lo \
	sqlrconnection/resumeresultset.d.lo \
	sqlrconnection/closeconnection.d.lo \
	sqlrconnection/closecursors.d.lo \
	sqlrconnection/temptables.d.lo \
	sqlrconnection/changeuser.d.lo \
	sqlrconnection/relogin.d.lo \
	sqlrconnection/markdatabasedavailable.d.lo \
	sqlrconnection/getuser.d.lo \
	sqlrconnection/getpassword.d.lo \
	sqlrconnection/connectstringvalue.d.lo \
	sqlrconnection/istransactional.d.lo \
	sqlrconnection/bindparameters.d.lo \
	sqlrconnection/unixsocketseqfile.d.lo \
	sqlrcursor/sqlrcursorctor.d.lo \
	sqlrcursor/sqlrcursordtor.d.lo \
	sqlrcursor/openclose.d.lo \
	sqlrcursor/preparequery.d.lo \
	sqlrcursor/queryparse.d.lo \
	sqlrcursor/binds.d.lo \
	sqlrcursor/fakebinds.d.lo \
	sqlrcursor/abort.d.lo \
	sqlrcursor/sid.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: libsqlrconnection.la libsqlrconnection_debug.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) */*.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) */*.o
	$(RMTREE) */.libs
	$(RMTREE) .libs

rebuild: clean all

libsqlrconnection.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libsqlrconnection_debug.la: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(DLOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrconnection.la $(libdir)
	$(LTINSTALL) libsqlrconnection_debug.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)
	$(MKINSTALLDIRS) $(tmpdir)
	chmod 777 $(tmpdir)
	$(MKINSTALLDIRS) $(tmpdir)/sockets
	chmod 777 $(tmpdir)/sockets
	$(MKINSTALLDIRS) $(tmpdir)/pids
	chmod 777 $(tmpdir)/pids
	$(MKINSTALLDIRS) $(tmpdir)/ipc
	chmod 777 $(tmpdir)/ipc
	$(MKINSTALLDIRS) $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection_debug.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection_p.la
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
