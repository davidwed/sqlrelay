include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

all: 
	if ( test -n "$(DB2LIBS)" ); then \
		$(MAKE) sqlr-connection-db2; \
		$(MAKE) sqlr-connection-db2-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-db2
	$(RM) sqlr-connection-db2-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(DB2INCLUDES) -c $<

db2connection_debug.o: db2connection.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(DB2INCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(DB2INCLUDES) -c $< -o $@

sqlr-connection-db2: db2connection.C db2connection.o main.C main.o 
	$(CXX) $(DB2STATIC) $(LDFLAGS) -o $@ db2connection.o main.o $(DB2LIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-db2-debug: db2connection.C db2connection_debug.o main.C main_debug.o 
	$(CXX) $(DB2STATIC) $(DEBUGLDFLAGS) -o $@ db2connection_debug.o main_debug.o $(DB2LIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(DB2LIBS)" ); then \
		$(INSTALL) sqlr-connection-db2 $(bindir); \
		$(INSTALL) sqlr-connection-db2-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-db2
	$(RM) $(bindir)/sqlr-connection-db2-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
