include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

all: 
	if ( test -n "$(ODBCLIBS)" ); then \
		$(MAKE) sqlr-connection-odbc; \
		$(MAKE) sqlr-connection-odbc-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-odbc
	$(RM) sqlr-connection-odbc-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ODBCINCLUDES) -c $<

odbcconnection_debug.o: odbcconnection.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(ODBCINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(ODBCINCLUDES) -c $< -o $@

sqlr-connection-odbc: odbcconnection.C odbcconnection.o main.C main.o 
	$(CXX) $(ODBCSTATIC) $(LDFLAGS) -o $@ odbcconnection.o main.o $(ODBCLIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-odbc-debug: odbcconnection.C odbcconnection_debug.o main.C main_debug.o 
	$(CXX) $(ODBCSTATIC) $(LDFLAGS) -o $@ odbcconnection_debug.o main_debug.o $(ODBCLIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(ODBCLIBS)" ); then \
		$(INSTALL) sqlr-connection-odbc $(bindir); \
		$(INSTALL) sqlr-connection-odbc-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-odbc
	$(RM) $(bindir)/sqlr-connection-odbc-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
