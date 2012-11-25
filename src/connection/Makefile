top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(SIDINCLUDES) $(DEBUGDIRCPPFLAG) $(LOGDIRCPPFLAG) -DSERVER_VERSION=\"$(SQLR_VERSION)\" -DLIBDIR=\"$(libdir)\"
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = sqlrcontroller/sqlrcontrollerctor.cpp \
	sqlrcontroller/sqlrcontrollerdtor.cpp \
	sqlrcontroller/flushwritebuffer.cpp \
	sqlrcontroller/init.cpp \
	sqlrcontroller/ipc.cpp \
	sqlrcontroller/opensockets.cpp \
	sqlrcontroller/listen.cpp \
	sqlrcontroller/listenercomm.cpp \
	sqlrcontroller/clientsession.cpp \
	sqlrcontroller/sessionqueries.cpp \
	sqlrcontroller/authenticate.cpp \
	sqlrcontroller/suspendsession.cpp \
	sqlrcontroller/endsession.cpp \
	sqlrcontroller/selectdatabase.cpp \
	sqlrcontroller/getcurrentdatabase.cpp \
	sqlrcontroller/lastinsertid.cpp \
	sqlrcontroller/ping.cpp \
	sqlrcontroller/identify.cpp \
	sqlrcontroller/begin.cpp \
	sqlrcontroller/autocommit.cpp \
	sqlrcontroller/commit.cpp \
	sqlrcontroller/connectioncount.cpp \
	sqlrcontroller/rollback.cpp \
	sqlrcontroller/faketransactionblocks.cpp \
	sqlrcontroller/newquery.cpp \
	sqlrcontroller/getlist.cpp \
	sqlrcontroller/handlequery.cpp \
	sqlrcontroller/handlebinds.cpp \
	sqlrcontroller/rewritequery.cpp \
	sqlrcontroller/getbinds.cpp \
	sqlrcontroller/reexecutequery.cpp \
	sqlrcontroller/fetchfrombindcursor.cpp \
	sqlrcontroller/fetchresultset.cpp \
	sqlrcontroller/returnresultsetheader.cpp \
	sqlrcontroller/sendrowcounts.cpp \
	sqlrcontroller/returnoutputbindvalues.cpp \
	sqlrcontroller/sendcolumns.cpp \
	sqlrcontroller/returnresultsetdata.cpp \
	sqlrcontroller/sendfield.cpp \
	sqlrcontroller/abortresultset.cpp \
	sqlrcontroller/suspendresultset.cpp \
	sqlrcontroller/resumeresultset.cpp \
	sqlrcontroller/closeconnection.cpp \
	sqlrcontroller/closecursors.cpp \
	sqlrcontroller/temptables.cpp \
	sqlrcontroller/login.cpp \
	sqlrcontroller/markdatabasedavailable.cpp \
	sqlrcontroller/connectstringvalue.cpp \
	sqlrcontroller/unixsocketseqfile.cpp \
	sqlrcontroller/bindformat.cpp \
	sqlrcontroller/dbversion.cpp \
	sqlrcontroller/serverversion.cpp \
	sqlrcontroller/handlesignals.cpp \
	sqlrcontroller/main.cpp \
	sqlrcontroller/error.cpp \
	sqlrcontroller/user.cpp \
	sqlrcontroller/statistics.cpp \
	sqlrconnection/sqlrconnection.cpp \
	sqlrcursor/sqlrcursor.cpp \
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
	sqltriggers/sqltrigger.cpp \
	sqlrloggers/sqlrloggers.cpp \
	sqlrloggers/sqlrlogger.cpp \
	sqlrqueries/sqlrqueries.cpp \
	sqlrqueries/sqlrquery.cpp

LOBJS = sqlrcontroller/sqlrcontrollerctor.lo \
	sqlrcontroller/sqlrcontrollerdtor.lo \
	sqlrcontroller/flushwritebuffer.lo \
	sqlrcontroller/init.lo \
	sqlrcontroller/ipc.lo \
	sqlrcontroller/opensockets.lo \
	sqlrcontroller/listen.lo \
	sqlrcontroller/listenercomm.lo \
	sqlrcontroller/clientsession.lo \
	sqlrcontroller/sessionqueries.lo \
	sqlrcontroller/authenticate.lo \
	sqlrcontroller/suspendsession.lo \
	sqlrcontroller/endsession.lo \
	sqlrcontroller/selectdatabase.lo \
	sqlrcontroller/getcurrentdatabase.lo \
	sqlrcontroller/lastinsertid.lo \
	sqlrcontroller/ping.lo \
	sqlrcontroller/identify.lo \
	sqlrcontroller/begin.lo \
	sqlrcontroller/autocommit.lo \
	sqlrcontroller/commit.lo \
	sqlrcontroller/connectioncount.lo \
	sqlrcontroller/rollback.lo \
	sqlrcontroller/faketransactionblocks.lo \
	sqlrcontroller/newquery.lo \
	sqlrcontroller/getlist.lo \
	sqlrcontroller/handlequery.lo \
	sqlrcontroller/handlebinds.lo \
	sqlrcontroller/rewritequery.lo \
	sqlrcontroller/getbinds.lo \
	sqlrcontroller/reexecutequery.lo \
	sqlrcontroller/fetchfrombindcursor.lo \
	sqlrcontroller/fetchresultset.lo \
	sqlrcontroller/returnresultsetheader.lo \
	sqlrcontroller/sendrowcounts.lo \
	sqlrcontroller/returnoutputbindvalues.lo \
	sqlrcontroller/sendcolumns.lo \
	sqlrcontroller/returnresultsetdata.lo \
	sqlrcontroller/sendfield.lo \
	sqlrcontroller/abortresultset.lo \
	sqlrcontroller/suspendresultset.lo \
	sqlrcontroller/resumeresultset.lo \
	sqlrcontroller/closeconnection.lo \
	sqlrcontroller/closecursors.lo \
	sqlrcontroller/temptables.lo \
	sqlrcontroller/login.lo \
	sqlrcontroller/markdatabasedavailable.lo \
	sqlrcontroller/connectstringvalue.lo \
	sqlrcontroller/unixsocketseqfile.lo \
	sqlrcontroller/bindformat.lo \
	sqlrcontroller/dbversion.lo \
	sqlrcontroller/serverversion.lo \
	sqlrcontroller/handlesignals.lo \
	sqlrcontroller/main.lo \
	sqlrcontroller/error.lo \
	sqlrcontroller/user.lo \
	sqlrcontroller/statistics.lo \
	sqlrconnection/sqlrconnection.lo \
	sqlrcursor/sqlrcursor.lo \
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
	sqltriggers/sqltrigger.lo \
	sqlrloggers/sqlrloggers.lo \
	sqlrloggers/sqlrlogger.lo \
	sqlrqueries/sqlrqueries.lo \
	sqlrqueries/sqlrquery.lo

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
	libsqlrelay_sqltranslation_oracleize.la \
	libsqlrelay_sqltranslation_serialtoautoincrement.la \
	libsqlrelay_sqlrlogger_slowqueries.la \
	libsqlrelay_sqlrlogger_custom_nw.la \
	libsqlrelay_sqlrquery_sqlrcmdgstat.la \
	libsqlrelay_sqlrquery_sqlrcmdcstat.la

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
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltriggers/createtableautoincrementoracle.lo $(LDFLAGS) $(TRIGGERPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltrigger_droptableautoincrementoracle.la: sqltriggers/droptableautoincrementoracle.cpp sqltriggers/droptableautoincrementoracle.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltriggers/droptableautoincrementoracle.lo $(LDFLAGS) $(TRIGGERPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_doublequotestosinglequotes.la: sqltranslations/doublequotestosinglequotes.cpp sqltranslations/doublequotestosinglequotes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/doublequotestosinglequotes.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_informixtooracledate.la: sqltranslations/informixtooracledate.cpp sqltranslations/informixtooracledate.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/informixtooracledate.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_locksnowaitbydefault.la: sqltranslations/locksnowaitbydefault.cpp sqltranslations/locksnowaitbydefault.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/locksnowaitbydefault.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_oracletemptablespreserverowsbydefault.la: sqltranslations/oracletemptablespreserverowsbydefault.cpp sqltranslations/oracletemptablespreserverowsbydefault.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/oracletemptablespreserverowsbydefault.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_temptablesaddmissingcolumns.la: sqltranslations/temptablesaddmissingcolumns.cpp sqltranslations/temptablesaddmissingcolumns.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/temptablesaddmissingcolumns.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_temptableslocalize.la: sqltranslations/temptableslocalize.cpp sqltranslations/temptableslocalize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/temptableslocalize.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_translatedatetimes.la: sqltranslations/translatedatetimes.cpp sqltranslations/translatedatetimes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/translatedatetimes.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_matchestolike.la: sqltranslations/matchestolike.cpp sqltranslations/matchestolike.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/matchestolike.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_oracleize.la: sqltranslations/oracleize.cpp sqltranslations/oracleize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/oracleize.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqltranslation_serialtoautoincrement.la: sqltranslations/serialtoautoincrement.cpp sqltranslations/serialtoautoincrement.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/serialtoautoincrement.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqlrlogger_slowqueries.la: sqlrloggers/slowqueries.cpp sqlrloggers/slowqueries.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrloggers/slowqueries.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqlrlogger_custom_nw.la: sqlrloggers/custom_nw.cpp sqlrloggers/custom_nw.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrloggers/custom_nw.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqlrquery_sqlrcmdgstat.la: sqlrqueries/sqlrcmdgstat.cpp sqlrqueries/sqlrcmdgstat.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrqueries/sqlrcmdgstat.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

libsqlrelay_sqlrquery_sqlrcmdcstat.la: sqlrqueries/sqlrcmdcstat.cpp sqlrqueries/sqlrcmdcstat.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrqueries/sqlrcmdcstat.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined
	
install: install-connection install-triggers install-translations install-loggers install-queries

install-connection:
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
	$(MKINSTALLDIRS) $(logdir)
	chmod 777 $(logdir)
	$(MKINSTALLDIRS) $(debugdir)
	chmod 777 $(debugdir)

install-triggers:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrelay_sqltrigger_createtableautoincrementoracle.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltrigger_createtableautoincrementoracle.a
	$(LTINSTALL) libsqlrelay_sqltrigger_droptableautoincrementoracle.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltrigger_droptableautoincrementoracle.a
	$(LTINSTALL) libsqlrelay_sqltranslation_doublequotestosinglequotes.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

install-translations:
	$(MKINSTALLDIRS) $(libdir)
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
	$(LTINSTALL) libsqlrelay_sqltranslation_serialtoautoincrement.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqltranslation_serialtoautoincrement.a
	$(LIBTOOL) --mode=finish $(libdir)

install-loggers:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrelay_sqlrlogger_slowqueries.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqlrlogger_slowqueries.a
	$(LTINSTALL) libsqlrelay_sqlrlogger_custom_nw.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqlrlogger_custom_nw.a
	$(LIBTOOL) --mode=finish $(libdir)

install-queries:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrelay_sqlrquery_sqlrcmdgstat.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqlrquery_sqlrcmdgstat.a
	$(LTINSTALL) libsqlrelay_sqlrquery_sqlrcmdcstat.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqlrquery_sqlrcmdcstat.a
	$(LIBTOOL) --mode=finish $(libdir)

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
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqltranslation_serialtoautoincrement.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqlrlogger_slowqueries.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqlrlogger_custom_nw.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqlrquery_sqlrcmdgstat.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqlrquery_sqlrcmdcstat.la
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(logdir)
	$(RMTREE) $(debugdir)
