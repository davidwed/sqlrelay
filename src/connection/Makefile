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
	sqlrconnection/lastinsertid.C \
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
	sqlrconnection/isolationlevel.C \
	sqlrconnection/main.C \
	sqlrcursor/sqlrcursorctor.C \
	sqlrcursor/sqlrcursordtor.C \
	sqlrcursor/openclose.C \
	sqlrcursor/preparequery.C \
	sqlrcursor/queryparse.C \
	sqlrcursor/returnresultset.C \
	sqlrcursor/binds.C \
	sqlrcursor/fakebinds.C \
	sqlrcursor/translatequery.C \
	sqlrcursor/abort.C \
	sqlrcursor/sid.C \
	sqlparser/clauses.C \
	sqlparser/parse.C \
	sqlparser/parsecreate.C \
	sqlparser/parsedelete.C \
	sqlparser/parsedrop.C \
	sqlparser/parseinsert.C \
	sqlparser/parseselect.C \
	sqlparser/parseupdate.C \
	sqlparser/parseset.C \
	sqlparser/parsewhere.C \
	sqlparser/parsecomparison.C \
	sqlparser/parseexpression.C \
	sqlparser/sqlparser.C \
	sqlwriter/sqlwriter.C \
	sqlwriter/writecreate.C \
	sqlwriter/writedelete.C \
	sqlwriter/writedrop.C \
	sqlwriter/writegeneric.C \
	sqlwriter/writeinsert.C \
	sqlwriter/writeselect.C \
	sqlwriter/writewhere.C \
	sqlwriter/writeexpression.C \
	sqlwriter/writeupdate.C \
	sqlwriter/writeset.C \
	sqltranslator/sqltranslator.C
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
	sqlrconnection/lastinsertid.lo \
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
	sqlrconnection/isolationlevel.lo \
	sqlrconnection/main.lo \
	sqlrcursor/sqlrcursorctor.lo \
	sqlrcursor/sqlrcursordtor.lo \
	sqlrcursor/openclose.lo \
	sqlrcursor/preparequery.lo \
	sqlrcursor/queryparse.lo \
	sqlrcursor/returnresultset.lo \
	sqlrcursor/binds.lo \
	sqlrcursor/fakebinds.lo \
	sqlrcursor/translatequery.lo \
	sqlrcursor/abort.lo \
	sqlrcursor/sid.lo \
	sqlparser/clauses.lo \
	sqlparser/parse.lo \
	sqlparser/parsecreate.lo \
	sqlparser/parsedelete.lo \
	sqlparser/parsedrop.lo \
	sqlparser/parseinsert.lo \
	sqlparser/parseselect.lo \
	sqlparser/parseupdate.lo \
	sqlparser/parseset.lo \
	sqlparser/parsewhere.lo \
	sqlparser/parsecomparison.lo \
	sqlparser/parseexpression.lo \
	sqlparser/sqlparser.lo \
	sqlwriter/sqlwriter.lo \
	sqlwriter/writecreate.lo \
	sqlwriter/writedelete.lo \
	sqlwriter/writedrop.lo \
	sqlwriter/writegeneric.lo \
	sqlwriter/writeinsert.lo \
	sqlwriter/writeselect.lo \
	sqlwriter/writewhere.lo \
	sqlwriter/writeexpression.lo \
	sqlwriter/writeupdate.lo \
	sqlwriter/writeset.lo \
	sqltranslator/sqltranslator.lo

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
