include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../authenticator -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB)

all: 
	if ( test -n "$(FREETDSLIBS)" ); then \
		$(MAKE) sqlr-connection-freetds; \
		$(MAKE) sqlr-connection-freetds-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-freetds
	$(RM) sqlr-connection-freetds-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(FREETDSINCLUDES) -c $<

freetdsconnection_debug.o: freetdsconnection.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(FREETDSINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(FREETDSINCLUDES) -c $< -o $@

sqlr-connection-freetds: freetdsconnection.C freetdsconnection.o main.C main.o 
	$(CXX) $(FREETDSSTATIC) $(LDFLAGS) -o $@ freetdsconnection.o main.o $(FREETDSLIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-freetds-debug: freetdsconnection.C freetdsconnection_debug.o main.C main_debug.o 
	$(CXX) $(FREETDSSTATIC) $(LDFLAGS) -o $@ freetdsconnection_debug.o main_debug.o $(FREETDSLIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(FREETDSLIBS)" ); then \
		$(INSTALL) sqlr-connection-freetds $(bindir); \
		$(INSTALL) sqlr-connection-freetds-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-freetds
	$(RM) $(bindir)/sqlr-connection-freetds-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
