include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../../ -I../../common -I../../util -I../../connection $(RUDIMENTSINCLUDES)
LIBS = -L../../util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB)

all: 
	if ( test -n "$(SYBASELIBS)" ); then \
		$(MAKE) sqlr-connection-sybase; \
		$(MAKE) sqlr-connection-sybase-debug; \
	fi

clean:
	$(RM) *.o
	$(RM) sqlr-connection-sybase
	$(RM) sqlr-connection-sybase-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SYBASEINCLUDES) -c $<

sybaseconnection_debug.o: sybaseconnection.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(SYBASEINCLUDES) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) $(SYBASEINCLUDES) -c $< -o $@

sqlr-connection-sybase: sybaseconnection.C sybaseconnection.o main.C main.o 
	$(CXX) $(SYBASESTATIC) $(LDFLAGS) -o $@ sybaseconnection.o main.o $(SYBASELIBS) -L../../connection -lsqlrconnection $(LIBS)

sqlr-connection-sybase-debug: sybaseconnection.C sybaseconnection_debug.o main.C main_debug.o 
	$(CXX) $(SYBASESTATIC) $(LDFLAGS) -o $@ sybaseconnection_debug.o main_debug.o $(SYBASELIBS) -L../../connection -lsqlrconnection_debug $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	if ( test -n "$(SYBASELIBS)" ); then \
		$(INSTALL) sqlr-connection-sybase $(bindir); \
		$(INSTALL) sqlr-connection-sybase-debug $(bindir); \
	fi
	$(top_srcdir)/mkinstalldirs $(tmpdir)
	chmod 777 $(tmpdir)
	$(top_srcdir)/mkinstalldirs $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(RM) $(bindir)/sqlr-connection-sybase
	$(RM) $(bindir)/sqlr-connection-sybase-debug
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(debugdir)
