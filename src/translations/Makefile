top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/server -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqltranslation_doublequotestosinglequotes.la \
	sqltranslation_informixtooracledate.la \
	sqltranslation_locksnowaitbydefault.la \
	sqltranslation_temptablesaddmissingcolumns.la \
	sqltranslation_temptableslocalize.la \
	sqltranslation_translatedatetimes.la \
	sqltranslation_matchestolike.la \
	sqltranslation_oracleize.la \
	sqltranslation_serialtoautoincrement.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqltranslation_doublequotestosinglequotes.la: doublequotestosinglequotes.cpp doublequotestosinglequotes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ doublequotestosinglequotes.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_informixtooracledate.la: informixtooracledate.cpp informixtooracledate.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ informixtooracledate.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_locksnowaitbydefault.la: locksnowaitbydefault.cpp locksnowaitbydefault.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ locksnowaitbydefault.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_temptablesaddmissingcolumns.la: temptablesaddmissingcolumns.cpp temptablesaddmissingcolumns.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ temptablesaddmissingcolumns.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_temptableslocalize.la: temptableslocalize.cpp temptableslocalize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ temptableslocalize.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_translatedatetimes.la: translatedatetimes.cpp translatedatetimes.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ translatedatetimes.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_matchestolike.la: matchestolike.cpp matchestolike.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ matchestolike.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_oracleize.la: oracleize.cpp oracleize.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ oracleize.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltranslation_serialtoautoincrement.la: serialtoautoincrement.cpp serialtoautoincrement.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ serialtoautoincrement.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqltranslation_doublequotestosinglequotes.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_doublequotestosinglequotes.a
	$(RM) $(libexecdir)/sqltranslation_doublequotestosinglequotes.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_doublequotestosinglequotes.so so $(MODULESUFFIX)
	$(LTINSTALL) sqltranslation_informixtooracledate.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_informixtooracledate.a
	$(RM) $(libexecdir)/sqltranslation_informixtooracledate.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_informixtooracledate.so so $(MODULESUFFIX)
	$(LTINSTALL) sqltranslation_locksnowaitbydefault.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_locksnowaitbydefault.a
	$(RM) $(libexecdir)/sqltranslation_locksnowaitbydefault.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_locksnowaitbydefault.so so $(MODULESUFFIX)
	$(LTINSTALL) sqltranslation_temptablesaddmissingcolumns.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_temptablesaddmissingcolumns.a
	$(RM) $(libexecdir)/sqltranslation_temptablesaddmissingcolumns.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_temptablesaddmissingcolumns.so so $(MODULESUFFIX)
	$(LTINSTALL) sqltranslation_temptableslocalize.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_temptableslocalize.a
	$(RM) $(libexecdir)/sqltranslation_temptableslocalize.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_temptableslocalize.so so $(MODULESUFFIX)
	$(LTINSTALL) sqltranslation_translatedatetimes.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_translatedatetimes.a
	$(RM) $(libexecdir)/sqltranslation_translatedatetimes.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_translatedatetimes.so so $(MODULESUFFIX)
	$(LTINSTALL) sqltranslation_matchestolike.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_matchestolike.a
	$(RM) $(libexecdir)/sqltranslation_matchestolike.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_matchestolike.so so $(MODULESUFFIX)
	$(LTINSTALL) sqltranslation_oracleize.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_oracleize.a
	$(RM) $(libexecdir)/sqltranslation_oracleize.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_oracleize.so so $(MODULESUFFIX)
	$(LTINSTALL) sqltranslation_serialtoautoincrement.la $(libexecdir)
	$(RM) $(libexecdir)/sqltranslation_serialtoautoincrement.a
	$(RM) $(libexecdir)/sqltranslation_serialtoautoincrement.la
	$(MODULERENAME) $(libexecdir)/sqltranslation_serialtoautoincrement.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqltranslation_*
