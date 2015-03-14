top_builddir = ../../..

include ../../../config.mk

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
		sqlrqueryassignments.cpp
PLUGINLOBJS = $(shell ls ../../connections/*/*.lo) \
		$(shell ls ../../protocols/*.lo) \
		$(shell ls ../../parsers/*.lo) \
		$(shell ls ../../loggers/*.lo) \
		$(shell ls ../../pwdencs/*.lo) \
		$(shell ls ../../auths/*.lo) \
		$(shell ls ../../queries/*.lo)
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
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: $(PLUGINSRCS) \
	libsqlrserver.la \
	sqlr-start$(EXE) \
	sqlr-stop$(EXE) \
	sqlr-listener$(EXE) \
	sqlr-connection$(EXE) \
	sqlr-scaler$(EXE) \
	sqlr-cachemanager$(EXE)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest sqlr-start$(EXE) sqlr-stop$(EXE) sqlr-listener$(EXE) sqlr-connection$(EXE) sqlr-scaler$(EXE) sqlr-cachemanager$(EXE)
	if ( test -n "$(PLUGINSRCS)" ); \
	then \
		$(RM) $(PLUGINSRCS); \
	fi

libsqlrserver.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LINKFLAGS) -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(PLUGINLIBS) $(EXTRALIBS)

sqlr-start$(EXE): sqlr-start.cpp sqlr-start.lo
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-start.$(OBJ) $(LIBS) $(RPATH)

sqlr-stop$(EXE): sqlr-stop.cpp sqlr-stop.lo
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-stop.$(OBJ) $(LIBS) $(RPATH)

sqlr-listener$(EXE): sqlr-listener.cpp sqlr-listener.lo
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-listener.$(OBJ) $(LIBS) $(RPATH)

sqlr-connection$(EXE): sqlr-connection.cpp sqlr-connection.lo
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-connection.$(OBJ) $(LIBS) $(RPATH)

sqlr-scaler$(EXE): sqlr-scaler.cpp sqlr-scaler.lo
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-scaler.$(OBJ) $(LIBS) $(RPATH)

sqlr-cachemanager$(EXE): sqlr-cachemanager.cpp sqlr-cachemanager.lo
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-cachemanager.$(OBJ) $(LIBS) $(RPATH)

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

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) sqlr-start$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-stop$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-listener$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-connection$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-scaler$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-cachemanager$(EXE) $(bindir)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(INSTALL) -m 644 ../include/sqlrelay/sqlrserver.h $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrlistener.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrserverconnection.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrservercontroller.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrservercursor.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrserverdll.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrserverincludes.h $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 ../include/sqlrelay/private/sqlrshmdata.h $(includedir)/sqlrelay/private
	$(MKINSTALLDIRS) $(tmpdir)
	$(CHMOD) 777 $(tmpdir)
	$(MKINSTALLDIRS) $(tmpdir)/sockets
	$(CHMOD) 777 $(tmpdir)/sockets
	$(MKINSTALLDIRS) $(tmpdir)/pids
	$(CHMOD) 777 $(tmpdir)/pids
	$(MKINSTALLDIRS) $(tmpdir)/ipc
	$(CHMOD) 777 $(tmpdir)/ipc
	$(MKINSTALLDIRS) $(logdir)
	$(CHMOD) 777 $(logdir)
	$(MKINSTALLDIRS) $(debugdir)
	$(CHMOD) 777 $(debugdir)

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrserver.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrserver.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrserver.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RM) $(includedir)/sqlrelay/sqlrserver.h \
		$(includedir)/sqlrelay/private/sqlrlistener.h \
		$(includedir)/sqlrelay/private/sqlrserverconnection.h \
		$(includedir)/sqlrelay/private/sqlrservercontroller.h \
		$(includedir)/sqlrelay/private/sqlrservercursor.h \
		$(includedir)/sqlrelay/private/sqlrserverdll.h \
		$(includedir)/sqlrelay/private/sqlrserverincludes.h \
		$(includedir)/sqlrelay/private/sqlrshmdata.h \
		$(bindir)/sqlr-start$(EXE) \
		$(bindir)/sqlr-stop$(EXE) \
		$(bindir)/sqlr-listener$(EXE) \
		$(bindir)/sqlr-connection$(EXE) \
		$(bindir)/sqlr-scaler$(EXE) \
		$(bindir)/sqlr-cachemanager$(EXE)
	$(RMTREE) $(tmpdir) \
		$(prefix)/lib/sqlrelay/tmp \
		$(logdir) \
		$(debugdir)

uninstalldll:
	$(RM) $(bindir)/libsqlrserver.$(LIBEXT) \
		$(libdir)/libsqlrserver.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/libsqlrserver.$(LIBEXT)
