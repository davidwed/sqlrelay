top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(SIDINCLUDES) $(DEBUGDIRCPPFLAG) -DSERVER_VERSION=\"$(SQLR_VERSION)\" -DLIBDIR=\"$(libdir)\"
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = sqlrconnection/sqlrconnectionctor.cpp \
	sqlrconnection/sqlrconnectiondtor.cpp \
	sqlrconnection/flushwritebuffer.cpp \
	sqlrconnection/initconnection.cpp \
	sqlrconnection/ipc.cpp \
	sqlrconnection/opensockets.cpp \
	sqlrconnection/listen.cpp \
	sqlrconnection/listenercomm.cpp \
	sqlrconnection/clientsession.cpp \
	sqlrconnection/authenticate.cpp \
	sqlrconnection/suspendsession.cpp \
	sqlrconnection/endsession.cpp \
	sqlrconnection/selectdatabase.cpp \
	sqlrconnection/lastinsertid.cpp \
	sqlrconnection/ping.cpp \
	sqlrconnection/identify.cpp \
	sqlrconnection/begin.cpp \
	sqlrconnection/autocommit.cpp \
	sqlrconnection/commit.cpp \
	sqlrconnection/connectioncount.cpp \
	sqlrconnection/rollback.cpp \
	sqlrconnection/faketransactionblocks.cpp \
	sqlrconnection/newquery.cpp \
	sqlrconnection/getlist.cpp \
	sqlrconnection/handlequery.cpp \
	sqlrconnection/rewritequery.cpp \
	sqlrconnection/getbinds.cpp \
	sqlrconnection/getsendcolumninfo.cpp \
	sqlrconnection/reexecutequery.cpp \
	sqlrconnection/fetchfrombindcursor.cpp \
	sqlrconnection/fetchresultset.cpp \
	sqlrconnection/returnresultsetheader.cpp \
	sqlrconnection/sendrowcounts.cpp \
	sqlrconnection/returnoutputbindvalues.cpp \
	sqlrconnection/sendcolumns.cpp \
	sqlrconnection/returnresultsetdata.cpp \
	sqlrconnection/skiprows.cpp \
	sqlrconnection/sendfield.cpp \
	sqlrconnection/abortresultset.cpp \
	sqlrconnection/suspendresultset.cpp \
	sqlrconnection/resumeresultset.cpp \
	sqlrconnection/closeconnection.cpp \
	sqlrconnection/closecursors.cpp \
	sqlrconnection/temptables.cpp \
	sqlrconnection/changeuser.cpp \
	sqlrconnection/relogin.cpp \
	sqlrconnection/markdatabasedavailable.cpp \
	sqlrconnection/getuser.cpp \
	sqlrconnection/getpassword.cpp \
	sqlrconnection/connectstringvalue.cpp \
	sqlrconnection/istransactional.cpp \
	sqlrconnection/bindparameters.cpp \
	sqlrconnection/unixsocketseqfile.cpp \
	sqlrconnection/updatestats.cpp \
	sqlrconnection/bindformat.cpp \
	sqlrconnection/dbversion.cpp \
	sqlrconnection/serverversion.cpp \
	sqlrconnection/handlesignals.cpp \
	sqlrconnection/isolationlevel.cpp \
	sqlrconnection/main.cpp \
	sqlrcursor/sqlrcursorctor.cpp \
	sqlrcursor/sqlrcursordtor.cpp \
	sqlrcursor/openclose.cpp \
	sqlrcursor/preparequery.cpp \
	sqlrcursor/queryparse.cpp \
	sqlrcursor/returnresultset.cpp \
	sqlrcursor/binds.cpp \
	sqlrcursor/fakebinds.cpp \
	sqlrcursor/translatequery.cpp \
	sqlrcursor/abort.cpp \
	sqlrcursor/sid.cpp \
	sqlparser/clauses.cpp \
	sqlparser/parse.cpp \
	sqlparser/parsecreate.cpp \
	sqlparser/parsedelete.cpp \
	sqlparser/parsedrop.cpp \
	sqlparser/parseinsert.cpp \
	sqlparser/parseselect.cpp \
	sqlparser/parseupdate.cpp \
	sqlparser/parseset.cpp \
	sqlparser/parsefrom.cpp \
	sqlparser/parsewhere.cpp \
	sqlparser/parsecomparison.cpp \
	sqlparser/parseexpression.cpp \
	sqlparser/parselock.cpp \
	sqlparser/sqlparser.cpp \
	sqlwriter/sqlwriter.cpp \
	sqlwriter/writecreate.cpp \
	sqlwriter/writedelete.cpp \
	sqlwriter/writedrop.cpp \
	sqlwriter/writegeneric.cpp \
	sqlwriter/writeinsert.cpp \
	sqlwriter/writeselect.cpp \
	sqlwriter/writewhere.cpp \
	sqlwriter/writeexpression.cpp \
	sqlwriter/writeupdate.cpp \
	sqlwriter/writeset.cpp \
	sqlwriter/writelock.cpp \
	sqltranslations/sqltranslations.cpp \
	sqltranslations/sqltranslation.cpp \
	sqltriggers/sqltriggers.cpp \
	sqltriggers/sqltrigger.cpp

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
	sqlrconnection/begin.lo \
	sqlrconnection/autocommit.lo \
	sqlrconnection/commit.lo \
	sqlrconnection/connectioncount.lo \
	sqlrconnection/rollback.lo \
	sqlrconnection/faketransactionblocks.lo \
	sqlrconnection/newquery.lo \
	sqlrconnection/getlist.lo \
	sqlrconnection/handlequery.lo \
	sqlrconnection/rewritequery.lo \
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
	sqlparser/parsefrom.lo \
	sqlparser/parsewhere.lo \
	sqlparser/parsecomparison.lo \
	sqlparser/parseexpression.lo \
	sqlparser/parselock.lo \
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
	sqlwriter/writelock.lo \
	sqltranslations/sqltranslations.lo \
	sqltranslations/sqltranslation.lo \
	sqltriggers/sqltriggers.lo \
	sqltriggers/sqltrigger.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrconnection.la \
	libsqlrelay_sqltrigger_createtableautoincrementoracle.la \
	libsqlrelay_sqltrigger_droptableautoincrementoracle.la \
	libsqlrelay_sqltranslation_doublequotestosinglequotes.la \
	libsqlrelay_sqltranslation_informixtooracledate.la \
	libsqlrelay_sqltranslation_locksnowaitbydefault.la \
	libsqlrelay_sqltranslation_oracletemptablespreserverowsbydefault.la \
	libsqlrelay_sqltranslation_temptablesaddmissingcolumns.la \
	libsqlrelay_sqltranslation_temptableslocalize.la \
	libsqlrelay_sqltranslation_translatedatetimes.la \
	libsqlrelay_sqltranslation_matchestolike.la \
	libsqlrelay_sqltranslation_oracleize.la

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

libsqlrelay_sqltrigger_createtableautoincrementoracle.la: sqltriggers/createtableautoincrementoracle.cpp sqltriggers/createtableautoincrementoracle.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltriggers/createtableautoincrementoracle.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltrigger_droptableautoincrementoracle.la: sqltriggers/droptableautoincrementoracle.cpp sqltriggers/droptableautoincrementoracle.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltriggers/droptableautoincrementoracle.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_doublequotestosinglequotes.la: sqltranslations/doublequotestosinglequotes.cpp sqltranslations/doublequotestosinglequotes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/doublequotestosinglequotes.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_informixtooracledate.la: sqltranslations/informixtooracledate.cpp sqltranslations/informixtooracledate.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/informixtooracledate.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_locksnowaitbydefault.la: sqltranslations/locksnowaitbydefault.cpp sqltranslations/locksnowaitbydefault.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/locksnowaitbydefault.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_oracletemptablespreserverowsbydefault.la: sqltranslations/oracletemptablespreserverowsbydefault.cpp sqltranslations/oracletemptablespreserverowsbydefault.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/oracletemptablespreserverowsbydefault.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_temptablesaddmissingcolumns.la: sqltranslations/temptablesaddmissingcolumns.cpp sqltranslations/temptablesaddmissingcolumns.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/temptablesaddmissingcolumns.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_temptableslocalize.la: sqltranslations/temptableslocalize.cpp sqltranslations/temptableslocalize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/temptableslocalize.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_translatedatetimes.la: sqltranslations/translatedatetimes.cpp sqltranslations/translatedatetimes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/translatedatetimes.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_matchestolike.la: sqltranslations/matchestolike.cpp sqltranslations/matchestolike.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/matchestolike.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_oracleize.la: sqltranslations/oracleize.cpp sqltranslations/oracleize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/oracleize.lo $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined
	

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrconnection.la $(libdir)
	$(LTINSTALL) libsqlrelay_sqltrigger_createtableautoincrementoracle.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltrigger_createtableautoincrementoracle.a
	$(LTINSTALL) libsqlrelay_sqltrigger_droptableautoincrementoracle.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltrigger_droptableautoincrementoracle.a
	$(LTINSTALL) libsqlrelay_sqltranslation_doublequotestosinglequotes.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_doublequotestosinglequotes.a
	$(LTINSTALL) libsqlrelay_sqltranslation_informixtooracledate.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_informixtooracledate.a
	$(LTINSTALL) libsqlrelay_sqltranslation_locksnowaitbydefault.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_locksnowaitbydefault.a
	$(LTINSTALL) libsqlrelay_sqltranslation_oracletemptablespreserverowsbydefault.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_oracletemptablespreserverowsbydefault.a
	$(LTINSTALL) libsqlrelay_sqltranslation_temptablesaddmissingcolumns.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_temptablesaddmissingcolumns.a
	$(LTINSTALL) libsqlrelay_sqltranslation_temptableslocalize.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_temptableslocalize.a
	$(LTINSTALL) libsqlrelay_sqltranslation_translatedatetimes.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_translatedatetimes.a
	$(LTINSTALL) libsqlrelay_sqltranslation_matchestolike.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_matchestolike.a
	$(LTINSTALL) libsqlrelay_sqltranslation_oracleize.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_oracleize.a
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
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltrigger_createtableautoincrementoracle.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltrigger_droptableautoincrementoracle.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection_p.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_doublequotestosinglequotes.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_informixtooracledate.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_locksnowaitbydefault.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_oracletemptablespreserverowsbydefault.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_temptablesaddmissingcolumns.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_temptableslocalize.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_translatedatetimes.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_matchestolike.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_oracleize.la
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
