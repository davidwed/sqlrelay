include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

all: 
	if ( test -n "$(SQLITELIBS)" ); then \
		$(MAKE) sqlr-connection-sqlite; \
		$(MAKE) sqlr-connection-sqlite-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-sqlite
	$(RM) sqlr-connection-sqlite-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SQLITEINCLUDES) -c $<

sqliteconnection_debug.o: sqliteconnection.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(SQLITEINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(SQLITEINCLUDES) -c $< -o $@

sqlr-connection-sqlite: sqliteconnection.C sqliteconnection.o main.C main.o 
	$(CXX) $(SQLITESTATIC) $(LDFLAGS) -o $@ sqliteconnection.o main.o $(SQLITELIBS) $(PTHREADLIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-sqlite-debug: sqliteconnection.C sqliteconnection_debug.o main.C main_debug.o 
	$(CXX) $(SQLITESTATIC) $(DEBUGLDFLAGS) -o $@ sqliteconnection_debug.o main_debug.o $(SQLITELIBS) $(PTHREADLIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(SQLITELIBS)" ); then \
		$(INSTALL) sqlr-connection-sqlite $(bindir); \
		$(INSTALL) sqlr-connection-sqlite-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-sqlite
	$(RM) $(bindir)/sqlr-connection-sqlite-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
