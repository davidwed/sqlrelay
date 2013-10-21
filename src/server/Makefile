top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

ifeq ($(SQLRELAY_ENABLE_SHARED),no)
PLUGINSRCS = sqlrconnectiondeclarations.cpp \
		sqlrconnectionassignments.cpp \
		sqlrloggerdeclarations.cpp \
		sqlrloggerassignments.cpp \
		sqlrpwdencdeclarations.cpp \
		sqlrpwdencassignments.cpp \
		sqlrquerydeclarations.cpp \
		sqlrqueryassignments.cpp \
		sqltranslationdeclarations.cpp \
		sqltranslationassignments.cpp \
		sqltriggerdeclarations.cpp \
		sqltriggerassignments.cpp
PLUGINLOBJS = $(shell ls ../connections/*/*.lo) \
		$(shell ls ../loggers/*.lo) \
		$(shell ls ../pwdencs/*.lo) \
		$(shell ls ../queries/*.lo) \
		$(shell ls ../translations/*.lo) \
		$(shell ls ../triggers/*.lo)
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

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) $(CACHEDIRCPPFLAG) $(PSDEFINE) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = -L./ -lsqlrserver $(PLUGINLIBS) -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
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
	sqlrscaler.cpp \

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
	$(RM) $(PLUGINSRCS)
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

sqlrconnectiondeclarations.cpp: sqlrconnectiondeclarations.cpp.in
	$(RM) $@
	for file in `ls ../connections/*/*connection.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrconnectionassignments.cpp: sqlrconnectionassignments.cpp.in
	$(RM) $@
	for file in `ls ../connections/*/*connection.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrloggerdeclarations.cpp: sqlrloggerdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../loggers/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrloggerassignments.cpp: sqlrloggerassignments.cpp.in
	$(RM) $@
	for file in `ls ../loggers/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrpwdencdeclarations.cpp: sqlrpwdencdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../pwdencs/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrpwdencassignments.cpp: sqlrpwdencassignments.cpp.in
	$(RM) $@
	for file in `ls ../pwdencs/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrquerydeclarations.cpp: sqlrquerydeclarations.cpp.in
	$(RM) $@
	for file in `ls ../queries/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrqueryassignments.cpp: sqlrqueryassignments.cpp.in
	$(RM) $@
	for file in `ls ../queries/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqltranslationdeclarations.cpp: sqltranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../translations/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqltranslationassignments.cpp: sqltranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../translations/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqltriggerdeclarations.cpp: sqltriggerdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../triggers/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqltriggerassignments.cpp: sqltriggerassignments.cpp.in
	$(RM) $@
	for file in `ls ../triggers/*.lo`; \
	do \
		module=`basename $$file .lo`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

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
