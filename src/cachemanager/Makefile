include ../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -DCACHE_DIR=\"$(cachedir)\" -I./ -I$(abs_srcdir) -I$(abs_srcdir)/src/common $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(RUDIMENTSDEBUGLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = cachemanager.C main.C
NLOBJS = cachemanager.n.lo main.n.lo
PLOBJS = cachemanager.p.lo main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
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

sqlr-cachemanager: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS) -rpath $(libdir)

sqlr-cachemanager-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS) -rpath $(libdir)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-cachemanager $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-cachemanager-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager-debug
