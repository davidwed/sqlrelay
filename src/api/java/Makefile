top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean java rebuild install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(JAVAUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: java libSQLRConnection.la libSQLRCursor.la sqlrelay.jar

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) *.so
	$(RM) com/firstworks/sqlrelay/*.class
	$(RM) sqlrelay.jar
	$(RMTREE) .libs

rebuild: clean all

java:
	$(JAVAC) com/firstworks/sqlrelay/*.java

libSQLRConnection.la: com_firstworks_sqlrelay_SQLRConnection.C com_firstworks_sqlrelay_SQLRConnection.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ com_firstworks_sqlrelay_SQLRConnection.lo $(LDFLAGS) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(javadir)/com/firstworks/sqlrelay $(RPATH) -module -no-undefined

libSQLRCursor.la: com_firstworks_sqlrelay_SQLRCursor.C com_firstworks_sqlrelay_SQLRCursor.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ com_firstworks_sqlrelay_SQLRCursor.lo $(LDFLAGS) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) -rpath $(javadir)/com/firstworks/sqlrelay $(RPATH) -module -no-undefined

libSQLRConnection.so:
	if ( test -r .libs/libSQLRConnection.0.0.0 ); \
	then \
		cp .libs/libSQLRConnection.0.0.0 libSQLRConnection.so; \
	fi
	if ( test -r .libs/libSQLRConnection.0.0 ); \
	then \
		cp .libs/libSQLRConnection.0.0 libSQLRConnection.so; \
	fi
	if ( test -r .libs/libSQLRConnection.0 ); \
	then \
		cp .libs/libSQLRConnection.0 libSQLRConnection.so; \
	fi
	if ( test -r .libs/libSQLRConnection ); \
	then \
		cp .libs/libSQLRConnection libSQLRConnection.so; \
	fi
	if ( test -r .libs/libSQLRConnection.so.0.0.0 ); \
	then \
		cp .libs/libSQLRConnection.so.0.0.0 libSQLRConnection.so; \
	fi
	if ( test -r .libs/libSQLRConnection.so.0.0 ); \
	then \
		cp .libs/libSQLRConnection.so.0.0 libSQLRConnection.so; \
	fi
	if ( test -r .libs/libSQLRConnection.so.0 ); \
	then \
		cp .libs/libSQLRConnection.so.0 libSQLRConnection.so; \
	fi
	if ( test -r .libs/libSQLRConnection.so ); \
	then \
		cp .libs/libSQLRConnection.so libSQLRConnection.so; \
	fi

libSQLRCursor.so:
	if ( test -r .libs/libSQLRCursor.0.0.0 ); \
	then \
		cp .libs/libSQLRCursor.0.0.0 libSQLRCursor.so; \
	fi
	if ( test -r .libs/libSQLRCursor.0.0 ); \
	then \
		cp .libs/libSQLRCursor.0.0 libSQLRCursor.so; \
	fi
	if ( test -r .libs/libSQLRCursor.0 ); \
	then \
		cp .libs/libSQLRCursor.0 libSQLRCursor.so; \
	fi
	if ( test -r .libs/libSQLRCursor ); \
	then \
		cp .libs/libSQLRCursor libSQLRCursor.so; \
	fi
	if ( test -r .libs/libSQLRCursor.so.0.0.0 ); \
	then \
		cp .libs/libSQLRCursor.so.0.0.0 libSQLRCursor.so; \
	fi
	if ( test -r .libs/libSQLRCursor.so.0.0 ); \
	then \
		cp .libs/libSQLRCursor.so.0.0 libSQLRCursor.so; \
	fi
	if ( test -r .libs/libSQLRCursor.so.0 ); \
	then \
		cp .libs/libSQLRCursor.so.0 libSQLRCursor.so; \
	fi
	if ( test -r .libs/libSQLRCursor.so ); \
	then \
		cp .libs/libSQLRCursor.so libSQLRCursor.so; \
	fi

SQLRConnection.dll:
	if ( test -r .libs/cygSQLRConnection-0.dll ); \
	then \
		cp .libs/cygSQLRConnection-0.dll SQLRConnection.dll; \
	fi

SQLRCursor.dll:
	if ( test -r .libs/cygSQLRCursor-0.dll ); \
	then \
		cp .libs/cygSQLRCursor-0.dll SQLRCursor.dll; \
	fi

sqlrelay.jar:
	if ( test -n "$(JAR)" ); \
	then \
		$(JAR) -cf $@ com/firstworks/sqlrelay/SQLRConnection.class \
				com/firstworks/sqlrelay/SQLRCursor.class; \
	fi

ifeq ($(CYGWIN),)
install: libSQLRConnection.so libSQLRCursor.so
	$(MKINSTALLDIRS) $(javadir)/com/firstworks/sqlrelay
	$(LTINSTALL) libSQLRConnection.so $(javadir)/com/firstworks/sqlrelay
	$(LTINSTALL) libSQLRCursor.so $(javadir)/com/firstworks/sqlrelay
	if ( test -n "$(JAR)" ); \
	then \
		$(INSTALL) -m 644 sqlrelay.jar $(javadir); \
	else \
		$(INSTALL) -m 644 com/firstworks/sqlrelay/SQLRConnection.class $(javadir)/com/firstworks/sqlrelay; \
		$(INSTALL) -m 644 com/firstworks/sqlrelay/SQLRCursor.class $(javadir)/com/firstworks/sqlrelay; \
	fi
	$(RM) libSQLRConnection.so
	$(RM) libSQLRCursor.so
else
install: SQLRConnection.dll SQLRCursor.dll
	$(MKINSTALLDIRS) $(javadir)/com/firstworks/sqlrelay
	$(LTINSTALL) SQLRConnection.dll $(javadir)/com/firstworks/sqlrelay
	$(LTINSTALL) SQLRCursor.dll $(javadir)/com/firstworks/sqlrelay
	if ( test -n "$(JAR)" ); \
		$(INSTALL) -m 644 sqlrelay.jar $(javadir); \
	then \
		$(INSTALL) com/firstworks/sqlrelay/SQLRConnection.class $(javadir)/com/firstworks/sqlrelay; \
		$(INSTALL) com/firstworks/sqlrelay/SQLRCursor.class $(javadir)/com/firstworks/sqlrelay; \
	fi
	$(RM) SQLRConnection.dll
	$(RM) SQLRCursor.dll
endif

uninstall:
	$(RMTREE) $(javadir)/com/firstworks/sqlrelay
	$(RM) $(javadir)/sqlrelay.jar
