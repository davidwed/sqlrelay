top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean java install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(JAVAUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: java libSQLRConnection.la libSQLRCursor.la sqlrelay.jar

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o *.so
	$(RM) com/firstworks/sqlrelay/*.class sqlrelay.jar
	$(RMTREE) .libs

java:
	$(JAVAC) com/firstworks/sqlrelay/*.java

libSQLRConnection.la: com_firstworks_sqlrelay_SQLRConnection.cpp com_firstworks_sqlrelay_SQLRConnection.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ com_firstworks_sqlrelay_SQLRConnection.lo $(LDFLAGS) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(javadir)/com/firstworks/sqlrelay $(RPATH) -avoid-version -module -no-undefined

libSQLRCursor.la: com_firstworks_sqlrelay_SQLRCursor.cpp com_firstworks_sqlrelay_SQLRCursor.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ com_firstworks_sqlrelay_SQLRCursor.lo $(LDFLAGS) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(javadir)/com/firstworks/sqlrelay $(RPATH) -avoid-version -module -no-undefined

sqlrelay.jar:
	if ( test -n "$(JAR)" ); \
	then \
		$(JAR) -cf $@ com/firstworks/sqlrelay/SQLRConnection.class \
				com/firstworks/sqlrelay/SQLRCursor.class; \
	fi

install:
	$(MKINSTALLDIRS) $(javadir)/com/firstworks/sqlrelay
	$(LTINSTALL) libSQLRConnection.la $(javadir)/com/firstworks/sqlrelay
	$(RM) $(javadir)/com/firstworks/sqlrelay/libSQLRConnection.a
	$(RM) $(javadir)/com/firstworks/sqlrelay/libSQLRConnection.la
	$(LTINSTALL) libSQLRCursor.la $(javadir)/com/firstworks/sqlrelay
	$(RM) $(javadir)/com/firstworks/sqlrelay/libSQLRCursor.a
	$(RM) $(javadir)/com/firstworks/sqlrelay/libSQLRCursor.la
	if ( test -n "$(JAR)" ); \
	then \
		$(INSTALL) -m 644 sqlrelay.jar $(javadir); \
	else \
		$(INSTALL) -m 644 com/firstworks/sqlrelay/SQLRConnection.class $(javadir)/com/firstworks/sqlrelay; \
		$(INSTALL) -m 644 com/firstworks/sqlrelay/SQLRCursor.class $(javadir)/com/firstworks/sqlrelay; \
	fi

uninstall:
	$(RMTREE) $(javadir)/com/firstworks/sqlrelay
	$(RM) $(javadir)/sqlrelay.jar
