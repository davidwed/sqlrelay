include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB)

all: 
	if ( test -n "$(INTERBASELIBS)" ); then \
		$(MAKE) sqlr-connection-interbase; \
		$(MAKE) sqlr-connection-interbase-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-interbase
	$(RM) sqlr-connection-interbase-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INTERBASEINCLUDES) -c $<

interbaseconnection_debug.o: interbaseconnection.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(INTERBASEINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(INTERBASEINCLUDES) -c $< -o $@

sqlr-connection-interbase: interbaseconnection.C interbaseconnection.o main.C main.o 
	$(CXX) $(INTERBASESTATIC) $(LDFLAGS) -o $@ interbaseconnection.o main.o $(INTERBASELIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-interbase-debug: interbaseconnection.C interbaseconnection_debug.o main.C main_debug.o 
	$(CXX) $(INTERBASESTATIC) $(LDFLAGS) -o $@ interbaseconnection_debug.o main_debug.o $(INTERBASELIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(INTERBASELIBS)" ); then \
		$(INSTALL) sqlr-connection-interbase $(bindir); \
		$(INSTALL) sqlr-connection-interbase-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-interbase
	$(RM) $(bindir)/sqlr-connection-interbase-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
