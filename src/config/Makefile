include ../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common $(RUDIMENTSINCLUDES) `$(GTKCONFIG) --cflags`
#CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common $(RUDIMENTSINCLUDES) -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/X11R6/include
LIBS = $(RUDIMENTSLIBS) $(PTHREADLIBS) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(RUDIMENTSDEBUGLIBS) $(PTHREADLIBS) $(LIBDMALLOC) $(LIBEFENCE)

SRCS= configfile.C gtkfe.C
NLOBJS= configfile.n.lo gtkfe.n.lo
PLOBJS= configfile.p.lo gtkfe.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

#all: sqlr-config-gtk sqlr-config-gtk-debug
all: sqlr-config-gtk

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-config-gtk
	$(LIBTOOL) --mode=clean $(RM) sqlr-config-gtk-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-config-gtk: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(NLOBJS) `$(GTKCONFIG) --libs` $(LIBS)
	#$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(NLOBJS) -lgtk-x11 $(LIBS)

sqlr-config-gtk-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) `$(GTKCONFIG) --libs` $(DEBUGLIBS)
	#$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) -lgtk-x11 $(DEBUGLIBS)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-config-gtk $(bindir)
	#$(LIBTOOL) --mode=install $(INSTALL) sqlr-config-gtk-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-config-gtk
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-config-gtk-debug
