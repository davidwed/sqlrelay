top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) $(CACHEDIRCPPFLAG) $(PSDEFINE) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = sqlrlistener.cpp \
	sqlrcontroller.cpp \
	sqlrconnection.cpp \
	sqlrcursor.cpp \
	sqlrauthenticator.cpp \
	sqlparser.cpp \
	sqlwriter.cpp \
	sqltranslations.cpp \
	sqltranslation.cpp \
	sqltriggers.cpp \
	sqltrigger.cpp \
	sqlrloggers.cpp \
	sqlrlogger.cpp \
	sqlrqueries.cpp \
	sqlrquery.cpp \
	sqlrpwdencs.cpp \
	sqlrpwdenc.cpp \
	sqlrscaler.cpp

LOBJS = sqlrlistener.lo \
	sqlrcontroller.lo \
	sqlrconnection.lo \
	sqlrcursor.lo \
	sqlrauthenticator.lo \
	sqlparser.lo \
	sqlwriter.lo \
	sqltranslations.lo \
	sqltranslation.lo \
	sqltriggers.lo \
	sqltrigger.lo \
	sqlrloggers.lo \
	sqlrlogger.lo \
	sqlrqueries.lo \
	sqlrquery.lo \
	sqlrpwdencs.lo \
	sqlrpwdenc.lo \
	sqlrscaler.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrserver.la \
	sqlr-start$(EXE) \
	sqlr-listener$(EXE) \
	sqlr-connection$(EXE) \
	sqlr-scaler$(EXE) \
	sqlr-cachemanager$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o sqlr-start$(EXE) sqlr-listener$(EXE) sqlr-connection$(EXE) sqlr-scaler$(EXE) sqlr-cachemanager$(EXE)
	$(RMTREE) .libs

libsqlrserver.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

sqlr-start$(EXE): sqlr-start.cpp sqlr-start.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-start.lo -L./ -lsqlrserver $(LIBS) $(RPATH)

sqlr-listener$(EXE): sqlr-listener.cpp sqlr-listener.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-listener.lo -L./ -lsqlrserver $(LIBS) $(RPATH)

sqlr-connection$(EXE): sqlr-connection.cpp sqlr-connection.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-connection.lo -L./ -lsqlrserver $(LIBS) $(RPATH)

sqlr-scaler$(EXE): sqlr-scaler.cpp sqlr-scaler.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-scaler.lo -L./ -lsqlrserver $(LIBS) $(RPATH)

sqlr-cachemanager$(EXE): sqlr-cachemanager.cpp sqlr-cachemanager.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-cachemanager.lo -L./ -lsqlrserver $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrserver.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(INSTALL) -m 644 sqlrpwdenc.h $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-start$(EXE) $(bindir)
	$(LTINSTALL) sqlr-listener$(EXE) $(bindir)
	$(LTINSTALL) sqlr-connection$(EXE) $(bindir)
	$(LTINSTALL) sqlr-scaler$(EXE) $(bindir)
	$(LTINSTALL) sqlr-cachemanager$(EXE) $(bindir)
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

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrserver.la
	$(RM) $(includedir)/sqlrelay/sqlrpwdenc.h
	$(RM) $(bindir)/sqlr-start$(EXE)
	$(RM) $(bindir)/sqlr-listener$(EXE)
	$(RM) $(bindir)/sqlr-connection$(EXE)
	$(RM) $(bindir)/sqlr-scaler$(EXE)
	$(RM) $(bindir)/sqlr-cachemanager$(EXE)
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(logdir)
	$(RMTREE) $(debugdir)
	$(RM) $(libdir)/libsqlrelay_*
	$(RM) $(libexecdir)/libsqlrelay_*
	$(RM) $(libdir)/libsqlrconnection*
	$(RM) $(libdir)/libsqlrconnection_p*
