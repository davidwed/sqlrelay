include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

all: 
	if ( test -n "$(ORACLELIBS)" ); then \
		$(MAKE) sqlr-connection-oracle7; \
		$(MAKE) sqlr-connection-oracle7-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-oracle7
	$(RM) sqlr-connection-oracle7-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ORACLEINCLUDES) -c $<

oracle7connection_debug.o: oracle7connection.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(ORACLEINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(ORACLEINCLUDES) -c $< -o $@

sqlr-connection-oracle7: oracle7connection.C oracle7connection.o main.C main.o 
	$(CXX) $(ORACLESTATIC) $(LDFLAGS) -o $@ oracle7connection.o main.o $(ORACLELIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-oracle7-debug: oracle7connection.C oracle7connection_debug.o main.C main_debug.o 
	$(CXX) $(ORACLESTATIC) $(DEBUGLDFLAGS) -o $@ oracle7connection_debug.o main_debug.o $(ORACLELIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(ORACLELIBS)" ); then \
		$(INSTALL) sqlr-connection-oracle7 $(bindir); \
		$(INSTALL) sqlr-connection-oracle7-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-oracle7
	$(RM) $(bindir)/sqlr-connection-oracle7-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
