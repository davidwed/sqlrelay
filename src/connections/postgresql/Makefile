include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

all: 
	if ( test -n "$(POSTGRESQLLIBS)" ); then \
		$(MAKE) sqlr-connection-postgresql; \
		$(MAKE) sqlr-connection-postgresql-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-postgresql
	$(RM) sqlr-connection-postgresql-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(POSTGRESQLINCLUDES) -c $<

postgresqlconnection_debug.o: postgresqlconnection.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(POSTGRESQLINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(POSTGRESQLINCLUDES) -c $< -o $@

sqlr-connection-postgresql: postgresqlconnection.o main.C main.o 
	$(CXX) $(POSTGRESQLSTATIC) $(LDFLAGS) -o $@ postgresqlconnection.o main.o $(POSTGRESQLLIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-postgresql-debug: postgresqlconnection_debug.o main.C main_debug.o 
	$(CXX) $(POSTGRESQLSTATIC) $(DEBUGLDFLAGS) -o $@ postgresqlconnection_debug.o main_debug.o $(POSTGRESQLLIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(POSTGRESQLLIBS)" ); then \
		$(INSTALL) sqlr-connection-postgresql $(bindir); \
		$(INSTALL) sqlr-connection-postgresql-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-postgresql
	$(RM) $(bindir)/sqlr-connection-postgresql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
