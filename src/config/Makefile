top_builddir = ../..

include ../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) $(SSLINCLUDES) `$(GTKCONFIG) --cflags`
#CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/X11R6/include
LIBS = $(RUDIMENTSLIBS) $(PTHREADLIBS) $(SSLLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS= configfile.C gtkfe.C
LOBJS= configfile.lo gtkfe.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

all: sqlr-config-gtk

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-config-gtk
	$(LIBTOOL) --mode=clean $(RM) sqlr-config-gtk-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-config-gtk: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(LOBJS) `$(GTKCONFIG) --libs` $(LIBS)
	#$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(LOBJS) -lgtk-x11 $(LIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(INSTALL) $(INSTALLSTRIP) sqlr-config-gtk $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-config-gtk
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-config-gtk-debug
