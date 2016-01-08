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
	sqlrmetadata.cpp \
	sqlrscaler.cpp \
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
	sqlrresultsettranslations.$(OBJ) \
	sqlrresultsettranslation.$(OBJ) \
	sqlrtriggers.$(OBJ) \
	sqlrtrigger.$(OBJ) \
	sqlrloggers.$(OBJ) \
	sqlrlogger.$(OBJ) \
	sqlrqueries.$(OBJ) \
	sqlrquery.$(OBJ) \
	sqlrpwdencs.$(OBJ) \
	sqlrpwdenc.$(OBJ) \
	sqlrauths.$(OBJ) \
	sqlrauth.$(OBJ) \
	sqlrmetadata.$(OBJ) \
	sqlrscaler.$(OBJ)

.SUFFIXES: .lo .cpp.in

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SERVERCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SERVERCPPFLAGS) $(COMPILE) $<

all: $(STATICPLUGINSRCS) \
	libsqlrserver.$(LIBEXT) \
	$(SQLR)-start$(EXE) \
	$(SQLR)-stop$(EXE) \
	$(SQLR)-listener$(EXE) \
	$(SQLR)-connection$(EXE) \
	$(SQLR)-scaler$(EXE) \
	$(SQLR)-cachemanager$(EXE)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest $(SQLR)-start$(EXE) $(SQLR)-stop$(EXE) $(SQLR)-listener$(EXE) $(SQLR)-connection$(EXE) $(SQLR)-scaler$(EXE) $(SQLR)-cachemanager$(EXE) $(STATICPLUGINSRCS)
	$(RMTREE) .libs

libsqlrserver.$(LIBEXT): $(LIBSQLRSERVERSRCS) $(LIBSQLRSERVERLOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LIBSQLRSERVERLOBJS) $(STATICPLUGINLOBJS) $(LDFLAGS) $(LINKFLAGS) $(LIBSQLRSERVERLIBS) $(RUDIMENTSLIBS) $(STATICPLUGINLIBS) $(EXTRALIBS)

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
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-cachemanager.$(OBJ) $(SERVERLIBS)

sqlrserverconnectiondeclarations.cpp: sqlrserverconnectiondeclarations.cpp.in
	$(RM) $@
	for file in `ls ../connections/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrserverconnectionassignments.cpp: sqlrserverconnectionassignments.cpp.in
	$(RM) $@
	for file in `ls ../connections/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrprotocoldeclarations.cpp: sqlrprotocoldeclarations.cpp.in
	$(RM) $@
	for file in `ls ../protocols/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrprotocolassignments.cpp: sqlrprotocolassignments.cpp.in
	$(RM) $@
	for file in `ls ../protocols/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrparserdeclarations.cpp: sqlrparserdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../parsers/*.$(OBJ) | grep -v sqlre`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrparserassignments.cpp: sqlrparserassignments.cpp.in
	$(RM) $@
	for file in `ls ../parsers/*.$(OBJ) | grep -v sqlre`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrmetadatadeclarations.cpp: sqlrmetadatadeclarations.cpp.in
	$(RM) $@
	for file in `ls ../metadata/*.$(OBJ) | grep -v sqlre`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrmetadataassignments.cpp: sqlrmetadataassignments.cpp.in
	$(RM) $@
	for file in `ls ../metadata/*.$(OBJ) | grep -v sqlre`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrloggerdeclarations.cpp: sqlrloggerdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../loggers/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrloggerassignments.cpp: sqlrloggerassignments.cpp.in
	$(RM) $@
	for file in `ls ../loggers/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrpwdencdeclarations.cpp: sqlrpwdencdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../pwdencs/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrpwdencassignments.cpp: sqlrpwdencassignments.cpp.in
	$(RM) $@
	for file in `ls ../pwdencs/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrauthdeclarations.cpp: sqlrauthdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../auths/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrauthassignments.cpp: sqlrauthassignments.cpp.in
	$(RM) $@
	for file in `ls ../auths/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtranslationdeclarations.cpp: sqlrtranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../translations/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrtranslationassignments.cpp: sqlrtranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../translations/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsettranslationdeclarations.cpp: sqlrresultsettranslationdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../resultsettranslations/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrresultsettranslationassignments.cpp: sqlrresultsettranslationassignments.cpp.in
	$(RM) $@
	for file in `ls ../resultsettranslations/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrfilterdeclarations.cpp: sqlrfilterdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../filters/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrfilterassignments.cpp: sqlrfilterassignments.cpp.in
	$(RM) $@
	for file in `ls ../filters/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrquerydeclarations.cpp: sqlrquerydeclarations.cpp.in
	$(RM) $@
	for file in `ls ../queries/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrqueryassignments.cpp: sqlrqueryassignments.cpp.in
	$(RM) $@
	for file in `ls ../queries/*.$(OBJ)`; \
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
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(CP) sqlrelay/sqlrserver.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrserver.h
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(CP) sqlrelay/private/sqlrlistener.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrlistener.h
	$(CP) sqlrelay/private/sqlrserverconnection.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrserverconnection.h
	$(CP) sqlrelay/private/sqlrservercontroller.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrservercontroller.h
	$(CP) sqlrelay/private/sqlrservercursor.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrservercursor.h
	$(CP) sqlrelay/private/sqlrserverdll.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrserverdll.h
	$(CP) sqlrelay/private/sqlrserverincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrserverincludes.h
	$(CP) sqlrelay/private/sqlrshmdata.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrshmdata.h
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
		$(bindir)/$(SQLR)-start$(EXE) \
		$(bindir)/$(SQLR)-stop$(EXE) \
		$(bindir)/$(SQLR)-listener$(EXE) \
		$(bindir)/$(SQLR)-connection$(EXE) \
		$(bindir)/$(SQLR)-scaler$(EXE) \
		$(bindir)/$(SQLR)-cachemanager$(EXE) \
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
