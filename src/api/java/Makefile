top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(JAVACPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(JAVACPPFLAGS) $(COMPILE) $<

all: com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRConnection.class com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRCursor.class libSQLRConnection.$(LIBEXT) libSQLRCursor.$(LIBEXT) sqlrelay.jar

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.log *.ii
	$(RM) com/firstworks/sqlrelay/*.class sqlrelay.jar
	$(RMTREE) .libs

com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRConnection.class: com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRConnection.java
	$(JAVAC) com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRConnection.java

com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRCursor.class: com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRCursor.java
	$(JAVAC) $(CURSORCLASSPATH) com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRCursor.java

libSQLRConnection.$(LIBEXT): com_firstworks_sqlrelay_SQLRConnection.cpp com_firstworks_sqlrelay_SQLRConnection.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ com_firstworks_sqlrelay_SQLRConnection.$(OBJ) $(LDFLAGS) $(JAVALIBS) $(MODLINKFLAGS)

libSQLRCursor.$(LIBEXT): com_firstworks_sqlrelay_SQLRCursor.cpp com_firstworks_sqlrelay_SQLRCursor.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ com_firstworks_sqlrelay_SQLRCursor.$(OBJ) $(LDFLAGS) $(JAVALIBS) $(JAVARPATH) $(MODLINKFLAGS)

sqlrelay.jar: com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRConnection.class com$(SLASH)firstworks$(SLASH)sqlrelay$(SLASH)SQLRCursor.class
	$(JAR) -cf $@ com/firstworks/sqlrelay/SQLRConnection.class com/firstworks/sqlrelay/SQLRCursor.class

install: $(INSTALLLIB)
	$(CP) sqlrelay.jar $(javadir)
	$(CHMOD) 644 $(javadir)/sqlrelay.jar

installdll:
	$(MKINSTALLDIRS) $(javadir)/com/firstworks/sqlrelay
	$(LTINSTALL) $(CP) libSQLRConnection.$(LIBEXT) $(javadir)/com/firstworks/sqlrelay/SQLRConnection.$(JNISUFFIX)
	$(LTINSTALL) $(CP) libSQLRCursor.$(LIBEXT) $(javadir)/com/firstworks/sqlrelay/SQLRCursor.$(JNISUFFIX)

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
