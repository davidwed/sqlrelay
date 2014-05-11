top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/server -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrtranslation_doublequotestosinglequotes.la \
	sqlrtranslation_informixtooracledate.la \
	sqlrtranslation_informixtomssqlserverdate.la \
	sqlrtranslation_locksnowaitbydefault.la \
	sqlrtranslation_locksmssqlserverize.la \
	sqlrtranslation_forupdatemssqlserverize.la \
	sqlrtranslation_temptablesaddmissingcolumns.la \
	sqlrtranslation_temptableslocalize.la \
	sqlrtranslation_temptablessybaseize.la \
	sqlrtranslation_temporarytoglobaltemporary.la \
	sqlrtranslation_uniquekeytounique.la \
	sqlrtranslation_oracleisolationlevel.la \
	sqlrtranslation_cascadetocascadeconstraints.la \
	sqlrtranslation_createselecttocreateasselect.la \
	sqlrtranslation_fakebindsforcreateasselect.la \
	sqlrtranslation_oracleunsupported.la \
	sqlrtranslation_mysqlunsupported.la \
	sqlrtranslation_translatedatetimes.la \
	sqlrtranslation_matchestolike.la \
	sqlrtranslation_oracleize.la \
	sqlrtranslation_serialtoautoincrement.la \
	sqlrtranslation_serialtoidentity.la \
	sqlrtranslation_show.la \
	sqlrtranslation_informixtomssqlserverselectinto.la \
	sqlrtranslation_concat.la \
	sqlrtranslation_uniquetodistinct.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrtranslation_doublequotestosinglequotes.la: doublequotestosinglequotes.cpp doublequotestosinglequotes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ doublequotestosinglequotes.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_informixtooracledate.la: informixtooracledate.cpp informixtooracledate.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ informixtooracledate.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_informixtomssqlserverdate.la: informixtomssqlserverdate.cpp informixtomssqlserverdate.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ informixtomssqlserverdate.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_locksnowaitbydefault.la: locksnowaitbydefault.cpp locksnowaitbydefault.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ locksnowaitbydefault.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_locksmssqlserverize.la: locksmssqlserverize.cpp locksmssqlserverize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ locksmssqlserverize.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_forupdatemssqlserverize.la: forupdatemssqlserverize.cpp forupdatemssqlserverize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ forupdatemssqlserverize.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_temptablesaddmissingcolumns.la: temptablesaddmissingcolumns.cpp temptablesaddmissingcolumns.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ temptablesaddmissingcolumns.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_temptableslocalize.la: temptableslocalize.cpp temptableslocalize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ temptableslocalize.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_temptablessybaseize.la: temptablessybaseize.cpp temptablessybaseize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ temptablessybaseize.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_temporarytoglobaltemporary.la: temporarytoglobaltemporary.cpp temporarytoglobaltemporary.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ temporarytoglobaltemporary.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_uniquekeytounique.la: uniquekeytounique.cpp uniquekeytounique.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ uniquekeytounique.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_oracleisolationlevel.la: oracleisolationlevel.cpp oracleisolationlevel.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ oracleisolationlevel.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_cascadetocascadeconstraints.la: cascadetocascadeconstraints.cpp cascadetocascadeconstraints.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ cascadetocascadeconstraints.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_createselecttocreateasselect.la: createselecttocreateasselect.cpp createselecttocreateasselect.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ createselecttocreateasselect.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_fakebindsforcreateasselect.la: fakebindsforcreateasselect.cpp fakebindsforcreateasselect.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ fakebindsforcreateasselect.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_oracleunsupported.la: oracleunsupported.cpp oracleunsupported.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ oracleunsupported.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_mysqlunsupported.la: mysqlunsupported.cpp mysqlunsupported.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ mysqlunsupported.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_translatedatetimes.la: translatedatetimes.cpp translatedatetimes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ translatedatetimes.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_matchestolike.la: matchestolike.cpp matchestolike.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ matchestolike.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_oracleize.la: oracleize.cpp oracleize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ oracleize.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_serialtoautoincrement.la: serialtoautoincrement.cpp serialtoautoincrement.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ serialtoautoincrement.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_serialtoidentity.la: serialtoidentity.cpp serialtoidentity.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ serialtoidentity.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_show.la: show.cpp show.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ show.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_informixtomssqlserverselectinto.la: informixtomssqlserverselectinto.cpp informixtomssqlserverselectinto.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ informixtomssqlserverselectinto.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_concat.la: concat.cpp concat.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ concat.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtranslation_uniquetodistinct.la: uniquetodistinct.cpp uniquetodistinct.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ uniquetodistinct.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrtranslation_doublequotestosinglequotes.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_doublequotestosinglequotes.a
	$(RM) $(libexecdir)/sqlrtranslation_doublequotestosinglequotes.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_doublequotestosinglequotes.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_informixtooracledate.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_informixtooracledate.a
	$(RM) $(libexecdir)/sqlrtranslation_informixtooracledate.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_informixtooracledate.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_informixtomssqlserverdate.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_informixtomssqlserverdate.a
	$(RM) $(libexecdir)/sqlrtranslation_informixtomssqlserverdate.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_informixtomssqlserverdate.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_locksnowaitbydefault.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_locksnowaitbydefault.a
	$(RM) $(libexecdir)/sqlrtranslation_locksnowaitbydefault.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_locksnowaitbydefault.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_locksmssqlserverize.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_locksmssqlserverize.a
	$(RM) $(libexecdir)/sqlrtranslation_locksmssqlserverize.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_locksmssqlserverize.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_forupdatemssqlserverize.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_forupdatemssqlserverize.a
	$(RM) $(libexecdir)/sqlrtranslation_forupdatemssqlserverize.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_forupdatemssqlserverize.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_temptablesaddmissingcolumns.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_temptablesaddmissingcolumns.a
	$(RM) $(libexecdir)/sqlrtranslation_temptablesaddmissingcolumns.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_temptablesaddmissingcolumns.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_temptableslocalize.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_temptableslocalize.a
	$(RM) $(libexecdir)/sqlrtranslation_temptableslocalize.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_temptableslocalize.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_temptablessybaseize.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_temptablessybaseize.a
	$(RM) $(libexecdir)/sqlrtranslation_temptablessybaseize.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_temptablessybaseize.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_temporarytoglobaltemporary.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_temporarytoglobaltemporary.a
	$(RM) $(libexecdir)/sqlrtranslation_temporarytoglobaltemporary.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_temporarytoglobaltemporary.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_uniquekeytounique.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_uniquekeytounique.a
	$(RM) $(libexecdir)/sqlrtranslation_uniquekeytounique.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_uniquekeytounique.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_oracleisolationlevel.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_oracleisolationlevel.a
	$(RM) $(libexecdir)/sqlrtranslation_oracleisolationlevel.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_oracleisolationlevel.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_cascadetocascadeconstraints.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_cascadetocascadeconstraints.a
	$(RM) $(libexecdir)/sqlrtranslation_cascadetocascadeconstraints.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_cascadetocascadeconstraints.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_createselecttocreateasselect.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_createselecttocreateasselect.a
	$(RM) $(libexecdir)/sqlrtranslation_createselecttocreateasselect.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_createselecttocreateasselect.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_fakebindsforcreateasselect.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_fakebindsforcreateasselect.a
	$(RM) $(libexecdir)/sqlrtranslation_fakebindsforcreateasselect.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_fakebindsforcreateasselect.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_oracleunsupported.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_oracleunsupported.a
	$(RM) $(libexecdir)/sqlrtranslation_oracleunsupported.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_oracleunsupported.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_mysqlunsupported.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_mysqlunsupported.a
	$(RM) $(libexecdir)/sqlrtranslation_mysqlunsupported.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_mysqlunsupported.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_translatedatetimes.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_translatedatetimes.a
	$(RM) $(libexecdir)/sqlrtranslation_translatedatetimes.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_translatedatetimes.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_matchestolike.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_matchestolike.a
	$(RM) $(libexecdir)/sqlrtranslation_matchestolike.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_matchestolike.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_oracleize.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_oracleize.a
	$(RM) $(libexecdir)/sqlrtranslation_oracleize.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_oracleize.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_serialtoautoincrement.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_serialtoautoincrement.a
	$(RM) $(libexecdir)/sqlrtranslation_serialtoautoincrement.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_serialtoautoincrement.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_serialtoidentity.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_serialtoidentity.a
	$(RM) $(libexecdir)/sqlrtranslation_serialtoidentity.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_serialtoidentity.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_show.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_show.a
	$(RM) $(libexecdir)/sqlrtranslation_show.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_show.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_informixtomssqlserverselectinto.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_informixtomssqlserverselectinto.a
	$(RM) $(libexecdir)/sqlrtranslation_informixtomssqlserverselectinto.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_informixtomssqlserverselectinto.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_concat.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_concat.a
	$(RM) $(libexecdir)/sqlrtranslation_concat.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_concat.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtranslation_uniquetodistinct.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtranslation_uniquetodistinct.a
	$(RM) $(libexecdir)/sqlrtranslation_uniquetodistinct.la
	$(MODULERENAME) $(libexecdir)/sqlrtranslation_uniquetodistinct.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqltranslation_*
	$(RM) $(libexecdir)/sqlrtranslation_*
