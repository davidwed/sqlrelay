include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -DCACHE_DIR=\"$(cachedir)\" -I./ -I../../ -I../common $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = cachemanager.C main.C
LOBJS = cachemanager.lo main.lo
PLOBJS = cachemanager.p.lo main.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-cachemanager sqlr-cachemanager-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-cachemanager
	$(LIBTOOL) --mode=clean $(RM) sqlr-cachemanager-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-cachemanager: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS)

sqlr-cachemanager-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) $(LIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-cachemanager $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-cachemanager-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager-debug
