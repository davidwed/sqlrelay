include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

all: 
	if ( test -n "$(MYSQLLIBS)" ); then \
		$(MAKE) sqlr-connection-mysql; \
		$(MAKE) sqlr-connection-mysql-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-mysql
	$(RM) sqlr-connection-mysql-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(MYSQLINCLUDES) -c $<

mysqlconnection_debug.o: mysqlconnection.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(MYSQLINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(MYSQLINCLUDES) -c $< -o $@

sqlr-connection-mysql: mysqlconnection.C mysqlconnection.o main.C main.o 
	$(CXX) $(MYSQLSTATIC) $(LDFLAGS) -o $@ mysqlconnection.o main.o $(MYSQLLIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-mysql-debug: mysqlconnection.C mysqlconnection_debug.o main.C main_debug.o 
	$(CXX) $(MYSQLSTATIC) $(DEBUGLDFLAGS) -o $@ mysqlconnection_debug.o main_debug.o $(MYSQLLIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(MYSQLLIBS)" ); then \
		$(INSTALL) sqlr-connection-mysql $(bindir); \
		$(INSTALL) sqlr-connection-mysql-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-mysql
	$(RM) $(bindir)/sqlr-connection-mysql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
