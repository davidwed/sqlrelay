include ../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -DCACHE_DIR=\"$(cachedir)\" -I./ -I$(abs_srcdir) -I$(abs_srcdir)/src/common $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = cachemanager.C main.C
LOBJS = cachemanager.lo main.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-cachemanager

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-cachemanager
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-cachemanager: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) -rpath $(libdir)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-cachemanager $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager-debug
