include ../../../config.mk

top_builddir = ../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../c++/include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: java libSQLRConnection.la libSQLRCursor.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) *.class

rebuild: clean all

java:
	$(JAVAC) *.java

libSQLRConnection.la: SQLRConnection.C SQLRConnection.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ SQLRConnection.lo -L../c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(javadir)/com/firstworks/sqlrelay -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libSQLRCursor.la: SQLRCursor.C SQLRCursor.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ SQLRCursor.lo -L../c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(top_builddir)/mkinstalldirs $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) -s libSQLRConnection.la $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) -s libSQLRCursor.la $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) SQLRConnection.class $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) SQLRCursor.class $(javadir)/com/firstworks/sqlrelay

uninstall:
	$(RMTREE) $(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=uninstall $(RM) $(javadir)/libSQLRConnection.la
	$(LIBTOOL) --mode=uninstall $(RM) $(javadir)/libSQLRCursor.la
	$(LIBTOOL) --mode=uninstall $(RM) $(javadir)/SQLRConnection.class
	$(LIBTOOL) --mode=uninstall $(RM) $(javadir)/SQLRCursor.class
