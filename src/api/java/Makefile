top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(JAVAINCLUDES)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(JAVAUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

all: java libSQLRConnection.la libSQLRCursor.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) *.so
	$(LIBTOOL) --mode=clean $(RM) com/firstworks/sqlrelay/*.class

rebuild: clean all

java:
	$(JAVAC) com/firstworks/sqlrelay/*.java

libSQLRConnection.la: com_firstworks_sqlrelay_SQLRConnection.C com_firstworks_sqlrelay_SQLRConnection.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ com_firstworks_sqlrelay_SQLRConnection.lo -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(PTHREADLIBS) -rpath $(javadir)/com/firstworks/sqlrelay $(RPATH) -module

libSQLRCursor.la: com_firstworks_sqlrelay_SQLRCursor.C com_firstworks_sqlrelay_SQLRCursor.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ com_firstworks_sqlrelay_SQLRCursor.lo -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(PTHREADLIBS) -rpath $(javadir)/com/firstworks/sqlrelay $(RPATH) -module

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

install: libSQLRConnection.so libSQLRCursor.so
	$(MKINSTALLDIRS) $(DESTDIR)$(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) -s libSQLRConnection.so $(DESTDIR)$(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) -s libSQLRCursor.so $(DESTDIR)$(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) com/firstworks/sqlrelay/SQLRConnection.class $(DESTDIR)$(javadir)/com/firstworks/sqlrelay
	$(LIBTOOL) --mode=install $(INSTALL) com/firstworks/sqlrelay/SQLRCursor.class $(DESTDIR)$(javadir)/com/firstworks/sqlrelay
	$(RM) libSQLRConnection.so
	$(RM) libSQLRCursor.so

uninstall:
	$(RMTREE) $(DESTDIR)$(javadir)/com/firstworks/sqlrelay
