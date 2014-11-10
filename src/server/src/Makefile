top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

ifeq ($(SQLRELAY_ENABLE_SHARED),no)
PLUGINSRCS = sqlrserverconnectiondeclarations.cpp \
		sqlrserverconnectionassignments.cpp \
		sqlrprotocoldeclarations.cpp \
		sqlrprotocolassignments.cpp \
		sqlrparserdeclarations.cpp \
		sqlrparserassignments.cpp \
		sqlrloggerdeclarations.cpp \
		sqlrloggerassignments.cpp \
		sqlrpwdencdeclarations.cpp \
		sqlrpwdencassignments.cpp \
		sqlrauthdeclarations.cpp \
		sqlrauthassignments.cpp \
		sqlrquerydeclarations.cpp \
		sqlrqueryassignments.cpp \
		sqlrtranslationdeclarations.cpp \
		sqlrtranslationassignments.cpp \
		sqlrresultsettranslationdeclarations.cpp \
		sqlrresultsettranslationassignments.cpp \
		sqlrtriggerdeclarations.cpp \
		sqlrtriggerassignments.cpp
PLUGINLOBJS = $(shell ls ../../connections/*/*.lo) \
		$(shell ls ../../protocols/*.lo) \
		$(shell ls ../../parsers/*.lo) \
		$(shell ls ../../loggers/*.lo) \
		$(shell ls ../../pwdencs/*.lo) \
		$(shell ls ../../auths/*.lo) \
		$(shell ls ../../queries/*.lo) \
		$(shell ls ../../translations/*.lo) \
		$(shell ls ../../resultsettranslations/*.lo) \
		$(shell ls ../../triggers/*.lo)
PLUGINLIBS =
ifneq ($(strip $(DB2LIBSPATH)),)
ifeq ($(DB2USERPATH),yes)
PLUGINLIBS += -R $(DB2LIBSPATH)
endif
endif
PLUGINLIBS += $(DB2LIBS)
ifneq ($(strip $(FIREBIRDLIBSPATH)),)
ifeq ($(FIREBIRDUSERPATH),yes)
PLUGINLIBS += -R $(FIREBIRDLIBSPATH)
endif
endif
PLUGINLIBS += $(FIREBIRDLIBS)
ifneq ($(strip $(FREETDSLIBSPATH)),)
ifeq ($(FREETDSUSERPATH),yes)
PLUGINLIBS += -R $(FREETDSLIBSPATH)
endif
endif
PLUGINLIBS += $(FREETDSLIBS)
ifneq ($(strip $(MDBTOOLSLIBSPATH)),)
ifeq ($(MDBTOOLSUSERPATH),yes)
PLUGINLIBS += -R $(MDBTOOLSLIBSPATH)
endif
endif
PLUGINLIBS += $(MDBTOOLSLIBS)
ifneq ($(strip $(MYSQLLIBSPATH)),)
ifeq ($(MYSQLUSERPATH),yes)
PLUGINLIBS += -R $(MYSQLLIBSPATH)
endif
endif
PLUGINLIBS += $(MYSQLLIBS)
ifneq ($(strip $(ODBCLIBSPATH)),)
ifeq ($(ODBCUSERPATH),yes)
PLUGINLIBS += -R $(ODBCLIBSPATH)
endif
endif
PLUGINLIBS += $(ODBCLIBS)
ifneq ($(strip $(ORACLELIBSPATH)),)
ifeq ($(ORACLEUSERPATH),yes)
PLUGINLIBS += -R $(ORACLELIBSPATH)
endif
endif
PLUGINLIBS += $(ORACLELIBS)
ifneq ($(strip $(POSTGRESQLLIBSPATH)),)
ifeq ($(POSTGRESQLUSERPATH),yes)
PLUGINLIBS += -R $(POSTGRESQLLIBSPATH)
endif
endif
PLUGINLIBS += $(POSTGRESQLLIBS)
ifneq ($(strip $(SQLITELIBSPATH)),)
ifeq ($(SQLITEUSERPATH),yes)
PLUGINLIBS += -R $(SQLITELIBSPATH)
endif
endif
PLUGINLIBS += $(SQLITELIBS)
ifneq ($(strip $(SYBASELIBSPATH)),)
ifeq ($(SYBASEUSERPATH),yes)
PLUGINLIBS += -R $(SYBASELIBSPATH)
endif
endif
PLUGINLIBS += $(SYBASELIBS)
PLUGINLIBS += -L$(top_builddir)/src/api/c++/src -lsqlrclient
PLUGINLIBS += -L$(top_builddir)/src/parsers -lsqlreparser
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) $(CACHEDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(LOGDIRCPPFLAG) -I../include -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = -L./ -lsqlrserver $(PLUGINLIBS) -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = sqlrlistener.cpp \
	sqlrservercontroller.cpp \
	sqlrserverconnection.cpp \
	sqlrservercursor.cpp \
	sqlrprotocols.cpp \
	sqlrprotocol.cpp \
	sqlrparser.cpp \
	sqlrtranslations.cpp \
	sqlrtranslation.cpp \
	sqlrresultsettranslations.cpp \
	sqlrresultsettranslation.cpp \
	sqlrtriggers.cpp \
	sqlrtrigger.cpp \
	sqlrloggers.cpp \
	sqlrlogger.cpp \
	sqlrqueries.cpp \
	sqlrquery.cpp \
	sqlrpwdencs.cpp \
	sqlrpwdenc.cpp \
	sqlrauths.cpp \
	sqlrauth.cpp \
	sqlrscaler.cpp \

LOBJS = sqlrlistener.lo \
	sqlrservercontroller.lo \
	sqlrserverconnection.lo \
	sqlrservercursor.lo \
	sqlrprotocols.lo \
	sqlrprotocol.lo \
	sqlrparser.lo \
	sqlrtranslations.lo \
	sqlrtranslation.lo \
	sqlrresultsettranslations.lo \
	sqlrresultsettranslation.lo \
	sqlrtriggers.lo \
	sqlrtrigger.lo \
	sqlrloggers.lo \
	sqlrlogger.lo \
	sqlrqueries.lo \
	sqlrquery.lo \
	sqlrpwdencs.lo \
	sqlrpwdenc.lo \
	sqlrauths.lo \
	sqlrauth.lo \
	sqlrscaler.lo \
	$(PLUGINLOBJS)

.SUFFIXES: .lo .cpp.in

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: $(PLUGINSRCS) \
	libsqlrserver.la \
	sqlr-start$(EXE) \
	sqlr-listener$(EXE) \
	sqlr-connection$(EXE) \
	sqlr-scaler$(EXE) \
	sqlr-cachemanager$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o sqlr-start$(EXE) sqlr-listener$(EXE) sqlr-connection$(EXE) sqlr-scaler$(EXE) sqlr-cachemanager$(EXE)
	if ( test -n "$(PLUGINSRCS)" ); \
	then \
		$(RM) $(PLUGINSRCS); \
	fi
	$(RMTREE) .libs

libsqlrserver.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(PLUGINLIBS) $(EXTRALIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

sqlr-start$(EXE): sqlr-start.cpp sqlr-start.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-start.lo $(LIBS) $(RPATH)

sqlr-listener$(EXE): sqlr-listener.cpp sqlr-listener.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-listener.lo $(LIBS) $(RPATH)

sqlr-connection$(EXE): sqlr-connection.cpp sqlr-connection.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-connection.lo $(LIBS) $(RPATH)

sqlr-scaler$(EXE): sqlr-scaler.cpp sqlr-scaler.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-scaler.lo $(LIBS) $(RPATH)

sqlr-cachemanager$(EXE): sqlr-cachemanager.cpp sqlr-cachemanager.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-cachemanager.lo $(LIBS) $(RPATH)

sqlrserverconnectiondeclarations.cpp: sqlrserverconnectiondeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../connections/*/*connection.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrserverconnectionassignments.cpp: sqlrserverconnectionassignments.cpp.in
	$(RM) $@
	for file in `ls ../../connections/*/*connection.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrprotocoldeclarations.cpp: sqlrprotocoldeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../protocols/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrprotocolassignments.cpp: sqlrprotocolassignments.cpp.in
	$(RM) $@
	for file in `ls ../../protocols/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrparserdeclarations.cpp: sqlrparserdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../parsers/*.lo | grep -v sqlre`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrparserassignments.cpp: sqlrparserassignments.cpp.in
	$(RM) $@
	for file in `ls ../../parsers/*.lo | grep -v sqlre`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrloggerdeclarations.cpp: sqlrloggerdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../loggers/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrloggerassignments.cpp: sqlrloggerassignments.cpp.in
	$(RM) $@
	for file in `ls ../../loggers/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrpwdencdeclarations.cpp: sqlrpwdencdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../pwdencs/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrpwdencassignments.cpp: sqlrpwdencassignments.cpp.in
	$(RM) $@
	for file in `ls ../../pwdencs/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrauthdeclarations.cpp: sqlrauthdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../auths/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrauthassignments.cpp: sqlrauthassignments.cpp.in
	$(RM) $@
	for file in `ls ../../auths/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrquerydeclarations.cpp: sqlrquerydeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../queries/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrqueryassignments.cpp: sqlrqueryassignments.cpp.in
	$(RM) $@
	for file in `ls ../../queries/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtranslationdeclarations.cpp: sqlrtranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../translations/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtranslationassignments.cpp: sqlrtranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../../translations/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsettranslationdeclarations.cpp: \
			sqlrresultsettranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../resultsettranslations/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsettranslationassignments.cpp: \
			sqlrresultsettranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../../resultsettranslations/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtriggerdeclarations.cpp: sqlrtriggerdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../../triggers/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtriggerassignments.cpp: sqlrtriggerassignments.cpp.in
	$(RM) $@
	for file in `ls ../../triggers/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

install:
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrserver.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)
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

installheaders:
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlparser.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrauth.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrauths.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrserverconnection.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrservercontroller.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrservercursor.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrlistener.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrlogger.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrloggers.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrprotocol.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrprotocols.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrpwdencs.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrqueries.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrquery.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrresultsettranslation.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrresultsettranslations.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrscaler.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrtranslation.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrtranslations.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrtrigger.h $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrtriggers.h $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrserverconnection.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrserverconnectionincludes.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrservercontroller.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrservercontrollerincludes.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrservercursor.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrservercursorincludes.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrlistener.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrlistenerincludes.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrserverdll.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrshmdata.h $(includedir)/sqlrelay/private

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrserver.la
	$(RM) $(includedir)/sqlrelay/sqlparser.h
	$(RM) $(includedir)/sqlrelay/sqlrauth.h
	$(RM) $(includedir)/sqlrelay/sqlrauths.h
	$(RM) $(includedir)/sqlrelay/sqlrserverconnection.h
	$(RM) $(includedir)/sqlrelay/sqlrservercontroller.h
	$(RM) $(includedir)/sqlrelay/sqlrservercursor.h
	$(RM) $(includedir)/sqlrelay/sqlrlistener.h
	$(RM) $(includedir)/sqlrelay/sqlrlogger.h
	$(RM) $(includedir)/sqlrelay/sqlrloggers.h
	$(RM) $(includedir)/sqlrelay/sqlrprotocol.h
	$(RM) $(includedir)/sqlrelay/sqlrprotocols.h
	$(RM) $(includedir)/sqlrelay/sqlrpwdenc.h
	$(RM) $(includedir)/sqlrelay/sqlrpwdencs.h
	$(RM) $(includedir)/sqlrelay/sqlrqueries.h
	$(RM) $(includedir)/sqlrelay/sqlrquery.h
	$(RM) $(includedir)/sqlrelay/sqlrresultsettranslation.h
	$(RM) $(includedir)/sqlrelay/sqlrresultsettranslations.h
	$(RM) $(includedir)/sqlrelay/sqlrscaler.h
	$(RM) $(includedir)/sqlrelay/sqlrtranslation.h
	$(RM) $(includedir)/sqlrelay/sqlrtranslations.h
	$(RM) $(includedir)/sqlrelay/sqlrtrigger.h
	$(RM) $(includedir)/sqlrelay/sqlrtriggers.h
	$(RM) $(includedir)/sqlrelay/private/sqlrserverdll.h
	$(RM) $(includedir)/sqlrelay/private/sqlrshmdata.h
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
