include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common -I../util $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) -L../util -lsqlrutil $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(RUDIMENTSDEBUGLIBS) -L../util -lsqlrutil_p $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = sqlrlistener.C main.C
LOBJS = sqlrlistener.lo main.lo
PLOBJS = sqlrlistener.p.lo main.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-listener sqlr-listener-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-listener
	$(LIBTOOL) --mode=clean $(RM) sqlr-listener-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-listener: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS)

sqlr-listener-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-listener $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-listener-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-listener
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-listener-debug
