top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(SIDINCLUDES) $(DEBUGDIRCPPFLAG) $(LOGDIRCPPFLAG) -DSERVER_VERSION=\"$(SQLR_VERSION)\" -DLIBEXECDIR=\"$(libexecdir)\"
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
	sqlrcontroller/error.cpp \
	sqlrcontroller/user.cpp \
	sqlrcontroller/statistics.cpp \
	sqlrcontroller/getquerytree.cpp \
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
	sqlrcontroller/sqltranslations.cpp \
	sqlrcontroller/sqltranslation.cpp \
	sqlrcontroller/sqltriggers.cpp \
	sqlrcontroller/sqltrigger.cpp \
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
	sqlrcontroller/error.lo \
	sqlrcontroller/user.lo \
	sqlrcontroller/statistics.lo \
	sqlrcontroller/getquerytree.lo \
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
	sqlrcontroller/sqltranslations.lo \
	sqlrcontroller/sqltranslation.lo \
	sqlrcontroller/sqltriggers.lo \
	sqlrcontroller/sqltrigger.lo \
	sqlrloggers/sqlrloggers.lo \
	sqlrloggers/sqlrlogger.lo \
	sqlrqueries/sqlrqueries.lo \
	sqlrqueries/sqlrquery.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrconnection.la \
	sqltranslation_doublequotestosinglequotes.la \
	sqltranslation_informixtooracledate.la \
	sqltranslation_locksnowaitbydefault.la \
	sqltranslation_temptablesaddmissingcolumns.la \
	sqltranslation_temptableslocalize.la \
	sqltranslation_translatedatetimes.la \
	sqltranslation_matchestolike.la \
	sqltranslation_oracleize.la \
	sqltranslation_serialtoautoincrement.la \
	sqlrlogger_slowqueries.la \
	sqlrlogger_custom_nw.la \
	sqlrquery_sqlrcmdgstat.la \
	sqlrquery_sqlrcmdcstat.la \
	sqlr-connection$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) */*.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) */*.o
	$(RMTREE) */.libs
	$(RMTREE) .libs

libsqlrconnection.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

sqltranslation_doublequotestosinglequotes.la: sqltranslations/doublequotestosinglequotes.cpp sqltranslations/doublequotestosinglequotes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/doublequotestosinglequotes.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_informixtooracledate.la: sqltranslations/informixtooracledate.cpp sqltranslations/informixtooracledate.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/informixtooracledate.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_locksnowaitbydefault.la: sqltranslations/locksnowaitbydefault.cpp sqltranslations/locksnowaitbydefault.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/locksnowaitbydefault.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_temptablesaddmissingcolumns.la: sqltranslations/temptablesaddmissingcolumns.cpp sqltranslations/temptablesaddmissingcolumns.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/temptablesaddmissingcolumns.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_temptableslocalize.la: sqltranslations/temptableslocalize.cpp sqltranslations/temptableslocalize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/temptableslocalize.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_translatedatetimes.la: sqltranslations/translatedatetimes.cpp sqltranslations/translatedatetimes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/translatedatetimes.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_matchestolike.la: sqltranslations/matchestolike.cpp sqltranslations/matchestolike.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/matchestolike.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_oracleize.la: sqltranslations/oracleize.cpp sqltranslations/oracleize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/oracleize.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_serialtoautoincrement.la: sqltranslations/serialtoautoincrement.cpp sqltranslations/serialtoautoincrement.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqltranslations/serialtoautoincrement.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrlogger_slowqueries.la: sqlrloggers/slowqueries.cpp sqlrloggers/slowqueries.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrloggers/slowqueries.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrlogger_custom_nw.la: sqlrloggers/custom_nw.cpp sqlrloggers/custom_nw.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrloggers/custom_nw.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrquery_sqlrcmdgstat.la: sqlrqueries/sqlrcmdgstat.cpp sqlrqueries/sqlrcmdgstat.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrqueries/sqlrcmdgstat.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrquery_sqlrcmdcstat.la: sqlrqueries/sqlrcmdcstat.cpp sqlrqueries/sqlrcmdcstat.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrqueries/sqlrcmdcstat.lo $(LDFLAGS) $(TRANSLATIONPLUGINLIBS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlr-connection$(EXE): sqlrcontroller/main.cpp sqlrcontroller/main.lo 
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrcontroller/main.lo $(LDFLAGS) libsqlrconnection.la $(LIBS) $(RPATH)
	
	
install: install-connection install-translations install-loggers install-queries

install-connection:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrconnection.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection$(EXE) $(bindir)
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

install-translations:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqltranslation_doublequotestosinglequotes.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_doublequotestosinglequotes.a
	$(RM) $(libexecdir)/sqltranslation_doublequotestosinglequotes.la
	$(LTINSTALL) sqltranslation_informixtooracledate.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_informixtooracledate.a
	$(RM) $(libexecdir)/sqltranslation_informixtooracledate.la
	$(LTINSTALL) sqltranslation_locksnowaitbydefault.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_locksnowaitbydefault.a
	$(RM) $(libexecdir)/sqltranslation_locksnowaitbydefault.la
	$(LTINSTALL) sqltranslation_temptablesaddmissingcolumns.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_temptablesaddmissingcolumns.a
	$(RM) $(libexecdir)/sqltranslation_temptablesaddmissingcolumns.la
	$(LTINSTALL) sqltranslation_temptableslocalize.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_temptableslocalize.a
	$(RM) $(libexecdir)/sqltranslation_temptableslocalize.la
	$(LTINSTALL) sqltranslation_translatedatetimes.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_translatedatetimes.a
	$(RM) $(libexecdir)/sqltranslation_translatedatetimes.la
	$(LTINSTALL) sqltranslation_matchestolike.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_matchestolike.a
	$(RM) $(libexecdir)/sqltranslation_matchestolike.la
	$(LTINSTALL) sqltranslation_oracleize.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_oracleize.a
	$(RM) $(libexecdir)/sqltranslation_oracleize.la
	$(LTINSTALL) sqltranslation_serialtoautoincrement.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_serialtoautoincrement.a
	$(RM) $(libexecdir)/sqltranslation_serialtoautoincrement.la
	$(LIBTOOL) --mode=finish $(libexecdir)

install-loggers:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrlogger_slowqueries.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrlogger_slowqueries.a
	$(RM) $(libexecdir)/sqlrlogger_slowqueries.la
	$(LTINSTALL) sqlrlogger_custom_nw.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrlogger_custom_nw.a
	$(RM) $(libexecdir)/sqlrlogger_custom_nw.la
	$(LIBTOOL) --mode=finish $(libexecdir)

install-queries:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrquery_sqlrcmdgstat.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.a
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.la
	$(LTINSTALL) sqlrquery_sqlrcmdcstat.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.a
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.la
	$(LIBTOOL) --mode=finish $(libexecdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection.la
	$(RM) $(libexecdir)/sqltranslation_*
	$(RM) $(libexecdir)/sqlrlogger_*
	$(RM) $(libexecdir)/sqlrquery_*
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(logdir)
	$(RMTREE) $(debugdir)
	$(RM) $(libdir)/libsqlrelay_*
	$(RM) $(libexecdir)/libsqlrelay_*
	$(RM) $(libdir)/libsqlrconnection_p.*
	$(RM) $(bindir)/sqlr-connection
