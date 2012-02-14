top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c++/include -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)


SRCS = connection/connectionctor.cpp \
	connection/connectiondtor.cpp \
	connection/connectioncopyreferences.cpp \
	connection/ping.cpp \
	connection/identify.cpp \
	connection/dbversion.cpp \
	connection/selectdatabase.cpp \
	connection/lastinsertid.cpp \
	connection/bindformat.cpp \
	connection/endsession.cpp \
	connection/suspendsession.cpp \
	connection/resumesession.cpp \
	connection/autocommit.cpp \
	connection/commit.cpp \
	connection/rollback.cpp \
	connection/serverversion.cpp \
	connection/debug.cpp \
	connection/error.cpp \
	connection/flushwritebuffer.cpp \
	connection/getreconnect.cpp \
	connection/getconnectionport.cpp \
	connection/getconnectionsocket.cpp \
	connection/authentication.cpp \
	connection/getnewport.cpp \
	connection/clearsessionflags.cpp \
	connection/clientversion.cpp \
	connection/opensession.cpp \
	connection/closeconnection.cpp \
	cursor/cursorctor.cpp \
	cursor/cursordtor.cpp \
	cursor/cursorcopyreferences.cpp \
	cursor/columns.cpp \
	cursor/fields.cpp \
	cursor/columnnamecase.cpp \
	cursor/nulls.cpp \
	cursor/suspendresultset.cpp \
	cursor/binds.cpp \
	cursor/fetchfrombindcursor.cpp \
	cursor/rowcounts.cpp \
	cursor/getcolumninfo.cpp \
	cursor/resultsetbuffersize.cpp \
	cursor/errormessage.cpp \
	cursor/readcache.cpp \
	cursor/writecache.cpp \
	cursor/resume.cpp \
	cursor/getresultsetid.cpp \
	cursor/preparequery.cpp \
	cursor/sendquery.cpp \
	cursor/executequery.cpp \
	cursor/runquery.cpp \
	cursor/receiveresultset.cpp \
	cursor/parsecolumninfo.cpp \
	cursor/parseoutputbinds.cpp \
	cursor/parsedata.cpp \
	cursor/clearresultset.cpp \
	cursor/skipandfetch.cpp \
	cursor/abortresultset.cpp \
	cursor/getlist.cpp \
	row/row.cpp
LOBJS = connection/connectionctor.lo \
	connection/connectiondtor.lo \
	connection/connectioncopyreferences.lo \
	connection/ping.lo \
	connection/identify.lo \
	connection/dbversion.lo \
	connection/selectdatabase.lo \
	connection/lastinsertid.lo \
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
	cursor/cursorcopyreferences.lo \
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

.cpp.lo:
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
