include ../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util $(RUDIMENTSINCLUDES)

SRCS = sqlrconnection/constructor.C \
	sqlrconnection/destructor.C \
	sqlrconnection/initconnection.C \
	sqlrconnection/opensockets.C \
	sqlrconnection/listen.C \
	sqlrconnection/clientsession.C \
	sqlrconnection/authenticate.C \
	sqlrconnection/suspendsession.C \
	sqlrconnection/endsession.C \
	sqlrconnection/ping.C \
	sqlrconnection/identify.C \
	sqlrconnection/autocommit.C \
	sqlrconnection/commit.C \
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
	connection/connectioncmdline.C \
	connection/ipc.C \
	connection/listenercomm.C \
	connection/scalercomm.C \
	connection/unixsocketseqfile.C \
	sqlrcursor/constructor.C \
	sqlrcursor/destructor.C \
	sqlrcursor/openclose.C \
	sqlrcursor/preparequery.C \
	sqlrcursor/queryparse.C \
	sqlrcursor/binds.C \
	sqlrcursor/fakebinds.C \
	sqlrcursor/abort.C
NLOBJS = sqlrconnection/constructor.n.lo \
	sqlrconnection/destructor.n.lo \
	sqlrconnection/initconnection.n.lo \
	sqlrconnection/opensockets.n.lo \
	sqlrconnection/listen.n.lo \
	sqlrconnection/clientsession.n.lo \
	sqlrconnection/authenticate.n.lo \
	sqlrconnection/suspendsession.n.lo \
	sqlrconnection/endsession.n.lo \
	sqlrconnection/ping.n.lo \
	sqlrconnection/identify.n.lo \
	sqlrconnection/autocommit.n.lo \
	sqlrconnection/commit.n.lo \
	sqlrconnection/rollback.n.lo \
	sqlrconnection/newquery.n.lo \
	sqlrconnection/handlequery.n.lo \
	sqlrconnection/handleerror.n.lo \
	sqlrconnection/getbinds.n.lo \
	sqlrconnection/getsendcolumninfo.n.lo \
	sqlrconnection/reexecutequery.n.lo \
	sqlrconnection/fetchfrombindcursor.n.lo \
	sqlrconnection/fetchresultset.n.lo \
	sqlrconnection/returnresultsetheader.n.lo \
	sqlrconnection/sendrowcounts.n.lo \
	sqlrconnection/returnoutputbindvalues.n.lo \
	sqlrconnection/sendcolumns.n.lo \
	sqlrconnection/returnresultsetdata.n.lo \
	sqlrconnection/skiprows.n.lo \
	sqlrconnection/sendfield.n.lo \
	sqlrconnection/abortresultset.n.lo \
	sqlrconnection/suspendresultset.n.lo \
	sqlrconnection/resumeresultset.n.lo \
	sqlrconnection/closeconnection.n.lo \
	sqlrconnection/closecursors.n.lo \
	sqlrconnection/temptables.n.lo \
	sqlrconnection/changeuser.n.lo \
	sqlrconnection/relogin.n.lo \
	sqlrconnection/markdatabasedavailable.n.lo \
	sqlrconnection/getuser.n.lo \
	sqlrconnection/getpassword.n.lo \
	sqlrconnection/connectstringvalue.n.lo \
	sqlrconnection/istransactional.n.lo \
	sqlrconnection/bindparameters.n.lo \
	connection/connectioncmdline.n.lo \
	connection/ipc.n.lo \
	connection/listenercomm.n.lo \
	connection/scalercomm.n.lo \
	connection/unixsocketseqfile.n.lo \
	sqlrcursor/constructor.n.lo \
	sqlrcursor/destructor.n.lo \
	sqlrcursor/openclose.n.lo \
	sqlrcursor/preparequery.n.lo \
	sqlrcursor/queryparse.n.lo \
	sqlrcursor/binds.n.lo \
	sqlrcursor/fakebinds.n.lo \
	sqlrcursor/abort.n.lo
PLOBJS = sqlrconnection/constructor.p.lo \
	sqlrconnection/destructor.p.lo \
	sqlrconnection/initconnection.p.lo \
	sqlrconnection/opensockets.p.lo \
	sqlrconnection/listen.p.lo \
	sqlrconnection/clientsession.p.lo \
	sqlrconnection/authenticate.p.lo \
	sqlrconnection/suspendsession.p.lo \
	sqlrconnection/endsession.p.lo \
	sqlrconnection/ping.p.lo \
	sqlrconnection/identify.p.lo \
	sqlrconnection/autocommit.p.lo \
	sqlrconnection/commit.p.lo \
	sqlrconnection/rollback.p.lo \
	sqlrconnection/newquery.p.lo \
	sqlrconnection/handlequery.p.lo \
	sqlrconnection/handleerror.p.lo \
	sqlrconnection/getbinds.p.lo \
	sqlrconnection/getsendcolumninfo.p.lo \
	sqlrconnection/reexecutequery.p.lo \
	sqlrconnection/fetchfrombindcursor.p.lo \
	sqlrconnection/fetchresultset.p.lo \
	sqlrconnection/returnresultsetheader.p.lo \
	sqlrconnection/sendrowcounts.p.lo \
	sqlrconnection/returnoutputbindvalues.p.lo \
	sqlrconnection/sendcolumns.p.lo \
	sqlrconnection/returnresultsetdata.p.lo \
	sqlrconnection/skiprows.p.lo \
	sqlrconnection/sendfield.p.lo \
	sqlrconnection/abortresultset.p.lo \
	sqlrconnection/suspendresultset.p.lo \
	sqlrconnection/resumeresultset.p.lo \
	sqlrconnection/closeconnection.p.lo \
	sqlrconnection/closecursors.p.lo \
	sqlrconnection/temptables.p.lo \
	sqlrconnection/changeuser.p.lo \
	sqlrconnection/relogin.p.lo \
	sqlrconnection/markdatabasedavailable.p.lo \
	sqlrconnection/getuser.p.lo \
	sqlrconnection/getpassword.p.lo \
	sqlrconnection/connectstringvalue.p.lo \
	sqlrconnection/istransactional.p.lo \
	sqlrconnection/bindparameters.p.lo \
	connection/connectioncmdline.p.lo \
	connection/ipc.p.lo \
	connection/listenercomm.p.lo \
	connection/scalercomm.p.lo \
	connection/unixsocketseqfile.p.lo \
	sqlrcursor/constructor.p.lo \
	sqlrcursor/destructor.p.lo \
	sqlrcursor/openclose.p.lo \
	sqlrcursor/preparequery.p.lo \
	sqlrcursor/queryparse.p.lo \
	sqlrcursor/binds.p.lo \
	sqlrcursor/fakebinds.p.lo \
	sqlrcursor/abort.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

#all: libsqlrconnection.la libsqlrconnection_p.la
all: libsqlrconnection.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) */*.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) */*.o

rebuild: clean all

libsqlrconnection.la: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(NLOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libsqlrconnection_p.la: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(PLOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)


install:
	$(abs_srcdir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -s libsqlrconnection.la $(libdir)
	#$(LIBTOOL) --mode=install $(INSTALL) libsqlrconnection_p.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection_p.la
