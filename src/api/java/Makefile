include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: java libSQLRConnection.la libSQLRCursor.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) com/firstworks/sqlrelay/*.class

rebuild: clean all

java:
	$(JAVAC) com/firstworks/sqlrelay/*.java

libSQLRConnection.la: com_firstworks_sqlrelay_SQLRConnection.C com_firstworks_sqlrelay_SQLRConnection.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ com_firstworks_sqlrelay_SQLRConnection.lo -L$(abs_srcdir)/src/api/c++/src -L$(abs_srcdir)/src/api/c++/src/.libs -lsqlrclient $(RUDIMENTSLIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libSQLRCursor.la: com_firstworks_sqlrelay_SQLRCursor.C com_firstworks_sqlrelay_SQLRCursor.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ com_firstworks_sqlrelay_SQLRCursor.lo -L$(abs_srcdir)/src/api/c++/src -L$(abs_srcdir)/src/api/c++/src/.libs -lsqlrclient $(RUDIMENTSLIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) -s libSQLRConnection.la $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) -s libSQLRCursor.la $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) com/firstworks/sqlrelay/SQLRConnection.class $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) com/firstworks/sqlrelay/SQLRCursor.class $(javadir)/com/firstworks/sqlrelay

uninstall:
	$(RMTREE) $(javadir)/com/firstworks/sqlrelay
