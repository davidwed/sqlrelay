top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(SIDINCLUDES) $(DEBUGDIRCPPFLAG) -DSERVER_VERSION=\"$(SQLR_VERSION)\"
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
	sqlrconnection/selectdatabase.C \
	sqlrconnection/ping.C \
	sqlrconnection/identify.C \
	sqlrconnection/autocommit.C \
	sqlrconnection/commit.C \
	sqlrconnection/connectioncount.C \
	sqlrconnection/rollback.C \
	sqlrconnection/faketransactionblocks.C \
	sqlrconnection/newquery.C \
	sqlrconnection/getlist.C \
	sqlrconnection/handlequery.C \
	sqlrconnection/rewritequery.C \
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
	sqlrconnection/updatestats.C \
	sqlrconnection/bindformat.C \
	sqlrconnection/dbversion.C \
	sqlrconnection/serverversion.C \
	sqlrconnection/handlesignals.C \
	sqlrconnection/main.C \
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
	sqlrconnection/selectdatabase.lo \
	sqlrconnection/ping.lo \
	sqlrconnection/identify.lo \
	sqlrconnection/autocommit.lo \
	sqlrconnection/commit.lo \
	sqlrconnection/connectioncount.lo \
	sqlrconnection/rollback.lo \
	sqlrconnection/faketransactionblocks.lo \
	sqlrconnection/newquery.lo \
	sqlrconnection/getlist.lo \
	sqlrconnection/handlequery.lo \
	sqlrconnection/rewritequery.lo \
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
	sqlrconnection/updatestats.lo \
	sqlrconnection/bindformat.lo \
	sqlrconnection/dbversion.lo \
	sqlrconnection/serverversion.lo \
	sqlrconnection/handlesignals.lo \
	sqlrconnection/main.lo \
	sqlrcursor/sqlrcursorctor.lo \
	sqlrcursor/sqlrcursordtor.lo \
	sqlrcursor/openclose.lo \
	sqlrcursor/preparequery.lo \
	sqlrcursor/queryparse.lo \
	sqlrcursor/binds.lo \
	sqlrcursor/fakebinds.lo \
	sqlrcursor/abort.lo \
	sqlrcursor/sid.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrconnection.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) */*.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) */*.o
	$(RMTREE) */.libs
	$(RMTREE) .libs

libsqlrconnection.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrconnection.la $(libdir)
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
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection_p.la
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
