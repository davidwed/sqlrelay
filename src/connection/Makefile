top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS)

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
	sqlrconnection.cpp \
	sqlrcursor.cpp \
	sqlparser.cpp \
	sqlwriter.cpp \
	sqltranslations.cpp \
	sqltranslation.cpp \
	sqltriggers.cpp \
	sqltrigger.cpp \
	sqlrloggers.cpp \
	sqlrlogger.cpp \
	sqlrqueries.cpp \
	sqlrquery.cpp

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
	sqlrconnection.lo \
	sqlrcursor.lo \
	sqlparser.lo \
	sqlwriter.lo \
	sqltranslations.lo \
	sqltranslation.lo \
	sqltriggers.lo \
	sqltrigger.lo \
	sqlrloggers.lo \
	sqlrlogger.lo \
	sqlrqueries.lo \
	sqlrquery.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrconnection.la \
	sqlr-connection$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo */*.lo *.o */*.o sqlr-connection$(EXE)
	$(RMTREE) */.libs .libs

libsqlrconnection.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

sqlr-connection$(EXE): sqlrcontroller/main.cpp sqlrcontroller/main.lo 
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlrcontroller/main.lo $(LDFLAGS) libsqlrconnection.la $(LIBS) $(RPATH)
	
	
install: install-connection install-loggers

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

install-loggers:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LIBTOOL) --mode=finish $(libexecdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection.la
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(logdir)
	$(RMTREE) $(debugdir)
	$(RM) $(libdir)/libsqlrelay_*
	$(RM) $(libexecdir)/libsqlrelay_*
	$(RM) $(libdir)/libsqlrconnection_p.*
	$(RM) $(bindir)/sqlr-connection
