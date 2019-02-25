top_builddir = ../..

include ../../config.mk

LIBSQLRSERVERSRCS = \
	sqlrlistener.cpp \
	sqlrservercontroller.cpp \
	sqlrserverconnection.cpp \
	sqlrservercursor.cpp \
	sqlrprotocols.cpp \
	sqlrprotocol.cpp \
	sqlrparser.cpp \
	sqlrtranslations.cpp \
	sqlrtranslation.cpp \
	sqlrfilters.cpp \
	sqlrfilter.cpp \
	sqlrbindvariabletranslations.cpp \
	sqlrbindvariabletranslation.cpp \
	sqlrresultsettranslations.cpp \
	sqlrresultsettranslation.cpp \
	sqlrresultsetrowtranslations.cpp \
	sqlrresultsetrowtranslation.cpp \
	sqlrresultsetrowblocktranslations.cpp \
	sqlrresultsetrowblocktranslation.cpp \
	sqlrresultsetheadertranslations.cpp \
	sqlrresultsetheadertranslation.cpp \
	sqlrtriggers.cpp \
	sqlrtrigger.cpp \
	sqlrloggers.cpp \
	sqlrlogger.cpp \
	sqlrnotifications.cpp \
	sqlrnotification.cpp \
	sqlrschedules.cpp \
	sqlrschedule.cpp \
	sqlrrouters.cpp \
	sqlrrouter.cpp \
	sqlrqueries.cpp \
	sqlrquery.cpp \
	sqlrpwdencs.cpp \
	sqlrpwdenc.cpp \
	sqlrcredentials.cpp \
	sqlrauths.cpp \
	sqlrauth.cpp \
	sqlrdirectives.cpp \
	sqlrdirective.cpp \
	sqlrmoduledatas.cpp \
	sqlrmoduledata.cpp \
	$(STATICPLUGINSRCS)

LIBSQLRSERVERLOBJS = \
	sqlrlistener.$(OBJ) \
	sqlrservercontroller.$(OBJ) \
	sqlrserverconnection.$(OBJ) \
	sqlrservercursor.$(OBJ) \
	sqlrprotocols.$(OBJ) \
	sqlrprotocol.$(OBJ) \
	sqlrparser.$(OBJ) \
	sqlrtranslations.$(OBJ) \
	sqlrtranslation.$(OBJ) \
	sqlrfilters.$(OBJ) \
	sqlrfilter.$(OBJ) \
	sqlrbindvariabletranslations.$(OBJ) \
	sqlrbindvariabletranslation.$(OBJ) \
	sqlrresultsettranslations.$(OBJ) \
	sqlrresultsettranslation.$(OBJ) \
	sqlrresultsetrowtranslations.$(OBJ) \
	sqlrresultsetrowtranslation.$(OBJ) \
	sqlrresultsetrowblocktranslations.$(OBJ) \
	sqlrresultsetrowblocktranslation.$(OBJ) \
	sqlrresultsetheadertranslations.$(OBJ) \
	sqlrresultsetheadertranslation.$(OBJ) \
	sqlrtriggers.$(OBJ) \
	sqlrtrigger.$(OBJ) \
	sqlrloggers.$(OBJ) \
	sqlrlogger.$(OBJ) \
	sqlrnotifications.$(OBJ) \
	sqlrnotification.$(OBJ) \
	sqlrschedules.$(OBJ) \
	sqlrschedule.$(OBJ) \
	sqlrrouters.$(OBJ) \
	sqlrrouter.$(OBJ) \
	sqlrqueries.$(OBJ) \
	sqlrquery.$(OBJ) \
	sqlrpwdencs.$(OBJ) \
	sqlrpwdenc.$(OBJ) \
	sqlrcredentials.$(OBJ) \
	sqlrauths.$(OBJ) \
	sqlrauth.$(OBJ) \
	sqlrdirectives.$(OBJ) \
	sqlrdirective.$(OBJ) \
	sqlrmoduledatas.$(OBJ) \
	sqlrmoduledata.$(OBJ)

.SUFFIXES: .lo .cpp.in

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SERVERCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SERVERCPPFLAGS) $(COMPILE) $<

all: $(STATICPLUGINSRCS) \
	lib$(SQLR)server.$(LIBEXT) \
	$(SQLR)-start$(EXE) \
	$(SQLR)-stop$(EXE) \
	$(SQLR)-listener$(EXE) \
	$(SQLR)-connection$(EXE) \
	$(SQLR)-scaler$(EXE) \
	$(SQLR)-cachemanager$(EXE) \
	$(SQLR)-pwdenc$(EXE) \
	$(SQLR)-status$(EXE)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii $(SQLR)-start$(EXE) $(SQLR)-stop$(EXE) $(SQLR)-listener$(EXE) $(SQLR)-connection$(EXE) $(SQLR)-scaler$(EXE) $(SQLR)-cachemanager$(EXE) $(SQLR)-pwdenc$(EXE) $(SQLR)-status$(EXE) $(STATICPLUGINSRCS)
	$(RMTREE) .libs

lib$(SQLR)server.$(LIBEXT): $(LIBSQLRSERVERSRCS) $(LIBSQLRSERVERLOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LIBSQLRSERVERLOBJS) $(STATICPLUGINLOBJS) $(LDFLAGS) $(SRVLINKFLAGS) $(LIBSQLRSERVERLIBS) $(RUDIMENTSLIBS) $(STATICPLUGINLIBS) $(EXTRALIBS)

$(SQLR)-start$(EXE): sqlr-start.cpp sqlr-start.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-start.$(OBJ) $(SERVERLIBS)

$(SQLR)-stop$(EXE): sqlr-stop.cpp sqlr-stop.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-stop.$(OBJ) $(SERVERLIBS)

$(SQLR)-listener$(EXE): sqlr-listener.cpp sqlr-listener.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-listener.$(OBJ) $(SERVERLIBS)

$(SQLR)-connection$(EXE): sqlr-connection.cpp sqlr-connection.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-connection.$(OBJ) $(SERVERLIBS)

$(SQLR)-scaler$(EXE): sqlr-scaler.cpp sqlr-scaler.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-scaler.$(OBJ) $(SERVERLIBS)

$(SQLR)-cachemanager$(EXE): sqlr-cachemanager.cpp sqlr-cachemanager.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-cachemanager.$(OBJ) $(CACHEMANAGERLIBS)

$(SQLR)-pwdenc$(EXE): sqlr-pwdenc.cpp sqlr-pwdenc.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-pwdenc.$(OBJ) $(SERVERLIBS)

$(SQLR)-status$(EXE): sqlr-status.cpp sqlr-status.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-status.$(OBJ) $(SERVERLIBS)

sqlrserverconnectiondeclarations.cpp: sqlrserverconnectiondeclarations.cpp.in
	$(RM) $@
	for file in `ls ../connections/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrserverconnectionassignments.cpp: sqlrserverconnectionassignments.cpp.in
	$(RM) $@
	for file in `ls ../connections/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrprotocoldeclarations.cpp: sqlrprotocoldeclarations.cpp.in
	$(RM) $@
	for file in `ls ../protocols/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrprotocolassignments.cpp: sqlrprotocolassignments.cpp.in
	$(RM) $@
	for file in `ls ../protocols/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrparserdeclarations.cpp: sqlrparserdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../parsers/*.$(OBJ) 2> /dev/null | grep -v sqlre`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrparserassignments.cpp: sqlrparserassignments.cpp.in
	$(RM) $@
	for file in `ls ../parsers/*.$(OBJ) 2> /dev/null | grep -v sqlre`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrloggerdeclarations.cpp: sqlrloggerdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../loggers/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrloggerassignments.cpp: sqlrloggerassignments.cpp.in
	$(RM) $@
	for file in `ls ../loggers/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrnotificationdeclarations.cpp: sqlrnotificationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../notifications/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrnotificationassignments.cpp: sqlrnotificationassignments.cpp.in
	$(RM) $@
	for file in `ls ../notifications/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrscheduledeclarations.cpp: sqlrscheduledeclarations.cpp.in
	$(RM) $@
	for file in `ls ../schedules/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrscheduleassignments.cpp: sqlrscheduleassignments.cpp.in
	$(RM) $@
	for file in `ls ../schedules/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrrouterdeclarations.cpp: sqlrrouterdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../routers/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrrouterassignments.cpp: sqlrrouterassignments.cpp.in
	$(RM) $@
	for file in `ls ../routers/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrpwdencdeclarations.cpp: sqlrpwdencdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../pwdencs/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrpwdencassignments.cpp: sqlrpwdencassignments.cpp.in
	$(RM) $@
	for file in `ls ../pwdencs/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrauthdeclarations.cpp: sqlrauthdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../auths/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrauthassignments.cpp: sqlrauthassignments.cpp.in
	$(RM) $@
	for file in `ls ../auths/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrdirectivedeclarations.cpp: sqlrdirectivedeclarations.cpp.in
	$(RM) $@
	for file in `ls ../directives/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrdirectiveassignments.cpp: sqlrdirectiveassignments.cpp.in
	$(RM) $@
	for file in `ls ../directives/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtranslationdeclarations.cpp: sqlrtranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../translations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtranslationassignments.cpp: sqlrtranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../translations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrbindvariabletranslationdeclarations.cpp: sqlrbindvariabletranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../bindvariabletranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrbindvariabletranslationassignments.cpp: sqlrbindvariabletranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../bindvariabletranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsettranslationdeclarations.cpp: sqlrresultsettranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../resultsettranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsettranslationassignments.cpp: sqlrresultsettranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../resultsettranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsetrowtranslationdeclarations.cpp: sqlrresultsetrowtranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../resultsetrowtranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsetrowtranslationassignments.cpp: sqlrresultsetrowtranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../resultsetrowtranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsetrowblocktranslationdeclarations.cpp: sqlrresultsetrowblocktranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../resultsetrowblocktranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsetrowblocktranslationassignments.cpp: sqlrresultsetrowblocktranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../resultsetrowblocktranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsetheadertranslationdeclarations.cpp: sqlrresultsetheadertranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../resultsetheadertranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsetheadertranslationassignments.cpp: sqlrresultsetheadertranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../resultsetheadertranslations/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrfilterdeclarations.cpp: sqlrfilterdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../filters/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrfilterassignments.cpp: sqlrfilterassignments.cpp.in
	$(RM) $@
	for file in `ls ../filters/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtriggerdeclarations.cpp: sqlrtriggerdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../triggers/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtriggerassignments.cpp: sqlrtriggerassignments.cpp.in
	$(RM) $@
	for file in `ls ../triggers/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrquerydeclarations.cpp: sqlrquerydeclarations.cpp.in
	$(RM) $@
	for file in `ls ../queries/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrqueryassignments.cpp: sqlrqueryassignments.cpp.in
	$(RM) $@
	for file in `ls ../queries/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrmoduledatadeclarations.cpp: sqlrmoduledatadeclarations.cpp.in
	$(RM) $@
	for file in `ls ../moduledatas/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrmoduledataassignments.cpp: sqlrmoduledataassignments.cpp.in
	$(RM) $@
	for file in `ls ../moduledatas/*.$(OBJ) 2> /dev/null`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-start$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-stop$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-listener$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-connection$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-scaler$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-cachemanager$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-pwdenc$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-status$(EXE) $(bindir)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(CP) sqlrelay/sqlrserver.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrserver.h
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(CP) sqlrelay/private/sqlrauth.h $(includedir)/sqlrelay/private/sqlrauth.h
	$(CP) sqlrelay/private/sqlrauths.h $(includedir)/sqlrelay/private/sqlrauths.h
	$(CP) sqlrelay/private/sqlrfilter.h $(includedir)/sqlrelay/private/sqlrfilter.h
	$(CP) sqlrelay/private/sqlrfilters.h $(includedir)/sqlrelay/private/sqlrfilters.h
	$(CP) sqlrelay/private/sqlrgsscredentials.h $(includedir)/sqlrelay/private/sqlrgsscredentials.h
	$(CP) sqlrelay/private/sqlrlistener.h $(includedir)/sqlrelay/private/sqlrlistener.h
	$(CP) sqlrelay/private/sqlrlogger.h $(includedir)/sqlrelay/private/sqlrlogger.h
	$(CP) sqlrelay/private/sqlrloggers.h $(includedir)/sqlrelay/private/sqlrloggers.h
	$(CP) sqlrelay/private/sqlrnotification.h $(includedir)/sqlrelay/private/sqlrnotification.h
	$(CP) sqlrelay/private/sqlrnotifications.h $(includedir)/sqlrelay/private/sqlrnotifications.h
	$(CP) sqlrelay/private/sqlrparser.h $(includedir)/sqlrelay/private/sqlrparser.h
	$(CP) sqlrelay/private/sqlrprotocol.h $(includedir)/sqlrelay/private/sqlrprotocol.h
	$(CP) sqlrelay/private/sqlrprotocols.h $(includedir)/sqlrelay/private/sqlrprotocols.h
	$(CP) sqlrelay/private/sqlrpwdenc.h $(includedir)/sqlrelay/private/sqlrpwdenc.h
	$(CP) sqlrelay/private/sqlrpwdencs.h $(includedir)/sqlrelay/private/sqlrpwdencs.h
	$(CP) sqlrelay/private/sqlrqueries.h $(includedir)/sqlrelay/private/sqlrqueries.h
	$(CP) sqlrelay/private/sqlrquerycursor.h $(includedir)/sqlrelay/private/sqlrquerycursor.h
	$(CP) sqlrelay/private/sqlrquery.h $(includedir)/sqlrelay/private/sqlrquery.h
	$(CP) sqlrelay/private/sqlrresultsetrowtranslation.h $(includedir)/sqlrelay/private/sqlrresultsetrowtranslation.h
	$(CP) sqlrelay/private/sqlrresultsetrowtranslations.h $(includedir)/sqlrelay/private/sqlrresultsetrowtranslations.h
	$(CP) sqlrelay/private/sqlrresultsetrowblocktranslation.h $(includedir)/sqlrelay/private/sqlrresultsetrowblocktranslation.h
	$(CP) sqlrelay/private/sqlrresultsetrowblocktranslations.h $(includedir)/sqlrelay/private/sqlrresultsetrowblocktranslations.h
	$(CP) sqlrelay/private/sqlrresultsetheadertranslation.h $(includedir)/sqlrelay/private/sqlrresultsetheadertranslation.h
	$(CP) sqlrelay/private/sqlrresultsetheadertranslations.h $(includedir)/sqlrelay/private/sqlrresultsetheadertranslations.h
	$(CP) sqlrelay/private/sqlrbindvariabletranslation.h $(includedir)/sqlrelay/private/sqlrbindvariabletranslation.h
	$(CP) sqlrelay/private/sqlrbindvariabletranslations.h $(includedir)/sqlrelay/private/sqlrbindvariabletranslations.h
	$(CP) sqlrelay/private/sqlrresultsettranslation.h $(includedir)/sqlrelay/private/sqlrresultsettranslation.h
	$(CP) sqlrelay/private/sqlrresultsettranslations.h $(includedir)/sqlrelay/private/sqlrresultsettranslations.h
	$(CP) sqlrelay/private/sqlrrouter.h $(includedir)/sqlrelay/private/sqlrrouter.h
	$(CP) sqlrelay/private/sqlrrouters.h $(includedir)/sqlrelay/private/sqlrrouters.h
	$(CP) sqlrelay/private/sqlrschedule.h $(includedir)/sqlrelay/private/sqlrschedule.h
	$(CP) sqlrelay/private/sqlrschedulerule.h $(includedir)/sqlrelay/private/sqlrschedulerule.h
	$(CP) sqlrelay/private/sqlrschedules.h $(includedir)/sqlrelay/private/sqlrschedules.h
	$(CP) sqlrelay/private/sqlrserverconnection.h $(includedir)/sqlrelay/private/sqlrserverconnection.h
	$(CP) sqlrelay/private/sqlrservercontroller.h $(includedir)/sqlrelay/private/sqlrservercontroller.h
	$(CP) sqlrelay/private/sqlrservercursor.h $(includedir)/sqlrelay/private/sqlrservercursor.h
	$(CP) sqlrelay/private/sqlrserverincludes.h $(includedir)/sqlrelay/private/sqlrserverincludes.h
	$(CP) sqlrelay/private/sqlrshm.h $(includedir)/sqlrelay/private/sqlrshm.h
	$(CP) sqlrelay/private/sqlrtlscredentials.h $(includedir)/sqlrelay/private/sqlrtlscredentials.h
	$(CP) sqlrelay/private/sqlrdirective.h $(includedir)/sqlrelay/private/sqlrdirective.h
	$(CP) sqlrelay/private/sqlrdirectives.h $(includedir)/sqlrelay/private/sqlrdirectives.h
	$(CP) sqlrelay/private/sqlrtranslation.h $(includedir)/sqlrelay/private/sqlrtranslation.h
	$(CP) sqlrelay/private/sqlrtranslations.h $(includedir)/sqlrelay/private/sqlrtranslations.h
	$(CP) sqlrelay/private/sqlrtrigger.h $(includedir)/sqlrelay/private/sqlrtrigger.h
	$(CP) sqlrelay/private/sqlrtriggers.h $(includedir)/sqlrelay/private/sqlrtriggers.h
	$(CP) sqlrelay/private/sqlruserpasswordcredentials.h $(includedir)/sqlrelay/private/sqlruserpasswordcredentials.h
	$(CP) sqlrelay/private/sqlrmoduledata.h $(includedir)/sqlrelay/private/sqlrmoduledata.h
	$(CP) sqlrelay/private/sqlrmoduledatas.h $(includedir)/sqlrelay/private/sqlrmoduledatas.h
	$(CP) sqlrelay/private/sqlrmysqlcredentials.h $(includedir)/sqlrelay/private/sqlrmysqlcredentials.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrauth.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrauths.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrfilter.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrfilters.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrgsscredentials.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrlistener.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrlogger.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrloggers.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrnotification.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrnotifications.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrparser.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrprotocols.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrpwdenc.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrpwdencs.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrqueries.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrquerycursor.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrquery.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsetrowtranslation.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsetrowtranslations.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsetrowblocktranslation.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsetrowblocktranslations.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsetheadertranslation.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsetheadertranslations.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrbindvariabletranslation.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrbindvariabletranslations.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsettranslation.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsettranslations.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrrouter.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrrouters.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrschedule.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrschedulerule.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrschedules.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrserverconnection.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrservercontroller.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrservercursor.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrserverincludes.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrshm.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrtlscredentials.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrtranslation.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrtranslations.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrtrigger.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrtriggers.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlruserpasswordcredentials.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrmoduledata.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrmoduledatas.h
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrmysqlcredentials.h
	$(MKINSTALLDIRS) $(tmpdir)
	$(CHMOD) 777 $(tmpdir)
	$(MKINSTALLDIRS) $(logdir)
	$(CHMOD) 777 $(logdir)
	$(MKINSTALLDIRS) $(debugdir)
	$(CHMOD) 777 $(debugdir)

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) lib$(SQLR)server.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) lib$(SQLR)server.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) lib$(SQLR)server.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RM) $(includedir)/sqlrelay/sqlrserver.h \
		$(includedir)/sqlrelay/private/sqlrauth.h \
		$(includedir)/sqlrelay/private/sqlrauths.h \
		$(includedir)/sqlrelay/private/sqlrfilter.h \
		$(includedir)/sqlrelay/private/sqlrfilters.h \
		$(includedir)/sqlrelay/private/sqlrgsscredentials.h \
		$(includedir)/sqlrelay/private/sqlrlistener.h \
		$(includedir)/sqlrelay/private/sqlrlogger.h \
		$(includedir)/sqlrelay/private/sqlrloggers.h \
		$(includedir)/sqlrelay/private/sqlrnotification.h \
		$(includedir)/sqlrelay/private/sqlrnotifications.h \
		$(includedir)/sqlrelay/private/sqlrparser.h \
		$(includedir)/sqlrelay/private/sqlrprotocols.h \
		$(includedir)/sqlrelay/private/sqlrpwdenc.h \
		$(includedir)/sqlrelay/private/sqlrpwdencs.h \
		$(includedir)/sqlrelay/private/sqlrqueries.h \
		$(includedir)/sqlrelay/private/sqlrquerycursor.h \
		$(includedir)/sqlrelay/private/sqlrquery.h \
		$(includedir)/sqlrelay/private/sqlrresultsetheadertranslation.h \
		$(includedir)/sqlrelay/private/sqlrresultsetheadertranslations.h \
		$(includedir)/sqlrelay/private/sqlrresultsetrowtranslation.h \
		$(includedir)/sqlrelay/private/sqlrresultsetrowtranslations.h \
		$(includedir)/sqlrelay/private/sqlrresultsetrowblocktranslation.h \
		$(includedir)/sqlrelay/private/sqlrresultsetrowblocktranslations.h \
		$(includedir)/sqlrelay/private/sqlrbindvariabletranslation.h \
		$(includedir)/sqlrelay/private/sqlrbindvariabletranslations.h \
		$(includedir)/sqlrelay/private/sqlrresultsettranslation.h \
		$(includedir)/sqlrelay/private/sqlrresultsettranslations.h \
		$(includedir)/sqlrelay/private/sqlrrouter.h \
		$(includedir)/sqlrelay/private/sqlrrouters.h \
		$(includedir)/sqlrelay/private/sqlrschedule.h \
		$(includedir)/sqlrelay/private/sqlrschedulerule.h \
		$(includedir)/sqlrelay/private/sqlrschedules.h \
		$(includedir)/sqlrelay/private/sqlrserverconnection.h \
		$(includedir)/sqlrelay/private/sqlrservercontroller.h \
		$(includedir)/sqlrelay/private/sqlrservercursor.h \
		$(includedir)/sqlrelay/private/sqlrserverincludes.h \
		$(includedir)/sqlrelay/private/sqlrshm.h \
		$(includedir)/sqlrelay/private/sqlrtlscredentials.h \
		$(includedir)/sqlrelay/private/sqlrtranslation.h \
		$(includedir)/sqlrelay/private/sqlrtranslations.h \
		$(includedir)/sqlrelay/private/sqlrtrigger.h \
		$(includedir)/sqlrelay/private/sqlrtriggers.h \
		$(includedir)/sqlrelay/private/sqlruserpasswordcredentials.h \
		$(includedir)/sqlrelay/private/sqlrmoduledata.h \
		$(includedir)/sqlrelay/private/sqlrmoduledatas.h \
		$(includedir)/sqlrelay/private/sqlrmysqlcredentials.h \
		$(bindir)/$(SQLR)-start$(EXE) \
		$(bindir)/$(SQLR)-stop$(EXE) \
		$(bindir)/$(SQLR)-listener$(EXE) \
		$(bindir)/$(SQLR)-connection$(EXE) \
		$(bindir)/$(SQLR)-scaler$(EXE) \
		$(bindir)/$(SQLR)-cachemanager$(EXE) \
		$(bindir)/$(SQLR)-pwdenc$(EXE) \
		$(bindir)/$(SQLR)-status$(EXE) \
		$(bindir)/sqlr-start$(EXE) \
		$(bindir)/sqlr-stop$(EXE) \
		$(bindir)/sqlr-listener$(EXE) \
		$(bindir)/sqlr-connection$(EXE) \
		$(bindir)/sqlr-scaler$(EXE) \
		$(bindir)/sqlr-cachemanager$(EXE) \
		$(bindir)/sqlr-pwdenc$(EXE) \
		$(bindir)/sqlr-status$(EXE)
	$(RMTREE) $(tmpdir) \
		$(logdir) \
		$(debugdir) \
		$(prefix)/lib/sqlrelay/tmp \
		$(localstatedir)/$(SQLRELAY) \
		$(localstatedir)/sqlrelay

uninstalldll:
	$(RM) $(bindir)/lib$(SQLR)server.$(LIBEXT) \
		$(libdir)/lib$(SQLR)server.lib \
		$(bindir)/lib$(SQLR)mysqlcredentials.$(LIBEXT) \
		$(libdir)/lib$(SQLR)mysqlcredentials.lib \
		$(bindir)/libsqlrserver.$(LIBEXT) \
		$(libdir)/libsqlrserver.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/lib$(SQLR)server.$(LIBEXT) \
				$(libdir)/lib$(SQLR)mysqlcredentials.$(LIBEXT) \
				 $(libdir)/libsqlrserver.$(LIBEXT)
