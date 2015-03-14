top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(WERROR) $(WNOUNKNOWNPRAGMAS) $(BASECPPFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(JAVAUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: java libSQLRConnection.$(LIBEXT) libSQLRCursor.$(LIBEXT) sqlrelay.jar

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RM) com/firstworks/sqlrelay/*.class sqlrelay.jar
	$(RMTREE) .libs

java:
	$(JAVAC) com/firstworks/sqlrelay/*.java

libSQLRConnection.$(LIBEXT): com_firstworks_sqlrelay_SQLRConnection.cpp com_firstworks_sqlrelay_SQLRConnection.lo
	$(LTLINK) $(LINK) $(OUT)$@ com_firstworks_sqlrelay_SQLRConnection.$(OBJ) $(LDFLAGS) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(javadir)/com/firstworks/sqlrelay $(RPATH) $(MODLINKFLAGS)

libSQLRCursor.$(LIBEXT): com_firstworks_sqlrelay_SQLRCursor.cpp com_firstworks_sqlrelay_SQLRCursor.lo
	$(LTLINK) $(LINK) $(OUT)$@ com_firstworks_sqlrelay_SQLRCursor.$(OBJ) $(LDFLAGS) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(javadir)/com/firstworks/sqlrelay $(RPATH) $(MODLINKFLAGS)

sqlrelay.jar:
	if ( test -n "$(JAR)" ); \
	then \
		$(JAR) -cf $@ com/firstworks/sqlrelay/SQLRConnection.class \
				com/firstworks/sqlrelay/SQLRCursor.class; \
	fi

install: $(INSTALLLIB)
	if ( test -n "$(JAR)" ); \
	then \
		$(CP) sqlrelay.jar $(javadir); \
		$(CHMOD) 644 $(javadir)/sqlrelay.jar; \
	else \
		$(CP) com/firstworks/sqlrelay/SQLRConnection.class $(javadir)/com/firstworks/sqlrelay; \
		$(CHMOD) 644 $(javadir)/com/firstworks/sqlrelay/SQLRConnection.class; \
		$(CP) com/firstworks/sqlrelay/SQLRCursor.class $(javadir)/com/firstworks/sqlrelay; \
		$(CHMOD) 644 $(javadir)/com/firstworks/sqlrelay/SQLRCursor.class; \
	fi

installdll:
	$(MKINSTALLDIRS) $(javadir)/com/firstworks/sqlrelay
	$(LTINSTALL) $(CP) libSQLRConnection.$(LIBEXT) $(javadir)/com/firstworks/sqlrelay/libSQLRConnection.$(JNISUFFIX)
	$(LTINSTALL) $(CP) libSQLRCursor.$(LIBEXT) $(javadir)/com/firstworks/sqlrelay/libSQLRCursor.$(JNISUFFIX)

installlib:
	$(MKINSTALLDIRS) $(javadir)/com/firstworks/sqlrelay
	$(LTINSTALL) $(CP) libSQLRConnection.$(LIBEXT) $(javadir)/com/firstworks/sqlrelay
	$(RM) $(javadir)/com/firstworks/sqlrelay/libSQLRConnection.a
	$(RM) $(javadir)/com/firstworks/sqlrelay/libSQLRConnection.$(LIBEXT)
	$(MODULERENAME) $(javadir)/com/firstworks/sqlrelay/libSQLRConnection.so so $(JNISUFFIX)
	$(LTINSTALL) $(CP) libSQLRCursor.la $(javadir)/com/firstworks/sqlrelay
	$(RM) $(javadir)/com/firstworks/sqlrelay/libSQLRCursor.a
	$(RM) $(javadir)/com/firstworks/sqlrelay/libSQLRCursor.$(LIBEXT)
	$(MODULERENAME) $(javadir)/com/firstworks/sqlrelay/libSQLRCursor.so so $(JNISUFFIX)

uninstall:
	$(RMTREE) $(javadir)/com/firstworks/sqlrelay
	$(RM) $(javadir)/sqlrelay.jar
