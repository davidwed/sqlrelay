include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(NSLLIB) $(LIBDMALLOC) $(LIBEFENCE)

all: 
	if ( test -n "$(LAGOLIBS)" ); then \
		$(MAKE) sqlr-connection-lago; \
		$(MAKE) sqlr-connection-lago-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-lago
	$(RM) sqlr-connection-lago-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LAGOINCLUDES) -c $<

lagoconnection_debug.o: lagoconnection.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(LAGOINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) $(LAGOINCLUDES) -c $< -o $@

sqlr-connection-lago: lagoconnection.C lagoconnection.o main.C main.o 
	$(CXX) $(LAGOSTATIC) $(LDFLAGS) -o $@ lagoconnection.o main.o $(LAGOLIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-lago-debug: lagoconnection.C lagoconnection_debug.o main.C main_debug.o 
	$(CXX) $(LAGOSTATIC) $(DEBUGLDFLAGS) -o $@ lagoconnection_debug.o main_debug.o $(LAGOLIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(LAGOLIBS)" ); then \
		$(INSTALL) sqlr-connection-lago $(bindir); \
		$(INSTALL) sqlr-connection-lago-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-lago
	$(RM) $(bindir)/sqlr-connection-lago-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
