include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common $(RUDIMENTSINCLUDES) `$(GTKCONFIG) --cflags`
LIBS = $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(RUDIMENTSDEBUGLIBS) $(LIBDMALLOC) $(LIBEFENCE)

SRCS= configfile.C gtkfe.C
NLOBJS= configfile.n.lo gtkfe.n.lo
PLOBJS= configfile.p.lo gtkfe.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-config-gtk sqlr-config-gtk-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-config-gtk
	$(LIBTOOL) --mode=clean $(RM) sqlr-config-gtk-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-config-gtk: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(NLOBJS) `$(GTKCONFIG) --libs` $(LIBS)

sqlr-config-gtk-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) `$(GTKCONFIG) --libs` $(DEBUGLIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-config-gtk $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-config-gtk-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-config-gtk
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-config-gtk-debug
