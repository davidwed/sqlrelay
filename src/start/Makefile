include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common -I../util $(RUDIMENTSINCLUDES) $(PSDEFINE)
LIBS = $(RUDIMENTSLIBS) -L../util -lsqlrutil $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = main.C
LOBJS = main.lo
PLOBJS = main.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-start sqlr-start-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-start
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-start: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS)

sqlr-start-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) $(LIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-start $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-start-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-start
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-start-debug
