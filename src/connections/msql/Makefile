include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(XNETLIB) $(LIBDMALLOC) $(LIBEFENCE)

all: 
	if ( test -n "$(MSQLLIBS)" ); then \
		$(MAKE) sqlr-connection-msql; \
		$(MAKE) sqlr-connection-msql-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-msql
	$(RM) sqlr-connection-msql-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(MSQLINCLUDES) -c $<

msqlconnection_debug.o: msqlconnection.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(MSQLINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(MSQLINCLUDES) -c $< -o $@

sqlr-connection-msql: msqlconnection.C msqlconnection.o main.C main.o 
	$(CXX) $(MSQLSTATIC) $(LDFLAGS) -o $@ msqlconnection.o main.o $(MSQLLIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-msql-debug: msqlconnection.C msqlconnection_debug.o main.C main_debug.o 
	$(CXX) $(MSQLSTATIC) $(LDFLAGS) -o $@ msqlconnection_debug.o main_debug.o $(MSQLLIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(MSQLLIBS)" ); then \
		$(INSTALL) sqlr-connection-msql $(bindir); \
		$(INSTALL) sqlr-connection-msql-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-msql
	$(RM) $(bindir)/sqlr-connection-msql-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
