include ../../config.mk

top_srcdir = ../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common -I../util $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) -L../util -lsqlrutil $(EXTRALIBS) $(SOCKETLIB)

SRCS = sqlrlistener.C main.C
OBJS = sqlrlistener.o main.o
DEBUGOBJS = sqlrlistener_debug.o main_debug.o

all: sqlr-listener sqlr-listener-debug

clean:
	$(RM) *.o
	$(RM) sqlr-listener
	$(RM) sqlr-listener-debug

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

sqlrlistener_debug.o: sqlrlistener.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

main_debug.o: main.C
	$(CXX) -DSERVER_DEBUG $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

sqlr-listener: $(SRCS) $(OBJS)
	$(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

sqlr-listener-debug: $(SRCS) $(DEBUGOBJS)
	$(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(DEBUGOBJS) $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	$(INSTALL) sqlr-listener $(bindir)
	$(INSTALL) sqlr-listener-debug $(bindir)

uninstall:
	$(RM) $(bindir)/sqlr-listener
	$(RM) $(bindir)/sqlr-listener-debug
