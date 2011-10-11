top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c++/include -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) -DCLIENT_VERSION=\"$(SQLR_VERSION)\"
LIBS = $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)


SRCS = connection/connectionctor.C \
	connection/connectiondtor.C \
	connection/copyreferences.C \
	connection/ping.C \
	connection/identify.C \
	connection/dbversion.C \
	connection/bindformat.C \
	connection/endsession.C \
	connection/suspendsession.C \
	connection/resumesession.C \
	connection/autocommit.C \
	connection/commit.C \
	connection/rollback.C \
	connection/serverversion.C \
	connection/debug.C \
	connection/error.C \
	connection/flushwritebuffer.C \
	connection/getreconnect.C \
	connection/getconnectionport.C \
	connection/getconnectionsocket.C \
	connection/authentication.C \
	connection/getnewport.C \
	connection/clearsessionflags.C \
	connection/clientversion.C \
	connection/opensession.C \
	connection/closeconnection.C \
	cursor/cursorctor.C \
	cursor/cursordtor.C \
	cursor/copyreferences.C \
	cursor/columns.C \
	cursor/fields.C \
	cursor/columnnamecase.C \
	cursor/nulls.C \
	cursor/suspendresultset.C \
	cursor/binds.C \
	cursor/fetchfrombindcursor.C \
	cursor/rowcounts.C \
	cursor/getcolumninfo.C \
	cursor/resultsetbuffersize.C \
	cursor/errormessage.C \
	cursor/readcache.C \
	cursor/writecache.C \
	cursor/resume.C \
	cursor/getresultsetid.C \
	cursor/preparequery.C \
	cursor/sendquery.C \
	cursor/executequery.C \
	cursor/runquery.C \
	cursor/receiveresultset.C \
	cursor/parsecolumninfo.C \
	cursor/parseoutputbinds.C \
	cursor/parsedata.C \
	cursor/clearresultset.C \
	cursor/skipandfetch.C \
	cursor/abortresultset.C \
	cursor/getlist.C \
	row/row.C
LOBJS = connection/connectionctor.lo \
	connection/connectiondtor.lo \
	connection/copyreferences.lo \
	connection/ping.lo \
	connection/identify.lo \
	connection/dbversion.lo \
	connection/bindformat.lo \
	connection/endsession.lo \
	connection/suspendsession.lo \
	connection/resumesession.lo \
	connection/autocommit.lo \
	connection/commit.lo \
	connection/rollback.lo \
	connection/serverversion.lo \
	connection/debug.lo \
	connection/error.lo \
	connection/flushwritebuffer.lo \
	connection/getreconnect.lo \
	connection/getconnectionport.lo \
	connection/getconnectionsocket.lo \
	connection/authentication.lo \
	connection/getnewport.lo \
	connection/clearsessionflags.lo \
	connection/clientversion.lo \
	connection/opensession.lo \
	connection/closeconnection.lo \
	cursor/cursorctor.lo \
	cursor/cursordtor.lo \
	cursor/copyreferences.lo \
	cursor/columns.lo \
	cursor/fields.lo \
	cursor/columnnamecase.lo \
	cursor/nulls.lo \
	cursor/suspendresultset.lo \
	cursor/binds.lo \
	cursor/fetchfrombindcursor.lo \
	cursor/rowcounts.lo \
	cursor/getcolumninfo.lo \
	cursor/resultsetbuffersize.lo \
	cursor/errormessage.lo \
	cursor/readcache.lo \
	cursor/writecache.lo \
	cursor/resume.lo \
	cursor/getresultsetid.lo \
	cursor/preparequery.lo \
	cursor/sendquery.lo \
	cursor/executequery.lo \
	cursor/runquery.lo \
	cursor/receiveresultset.lo \
	cursor/parsecolumninfo.lo \
	cursor/parseoutputbinds.lo \
	cursor/parsedata.lo \
	cursor/clearresultset.lo \
	cursor/skipandfetch.lo \
	cursor/abortresultset.lo \
	cursor/getlist.lo \
	row/row.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrclient.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) */*.lo
	$(LIBTOOL) --mode=clean $(RM) */*.o
	$(RMTREE) */.libs

libsqlrclient.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined $(LIBS)

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrclient.la $(libdir)/libsqlrclient.la
	$(LIBTOOL) --mode=finish $(libdir)
	$(MKINSTALLDIRS) $(cachedir)
	chmod 777 $(cachedir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclient.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrclient_p.la
	$(RMTREE) $(cachedir)
	$(RMTREE) $(prefix)/lib/sqlrelay/cache
