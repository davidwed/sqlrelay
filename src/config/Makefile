include ../../config.mk

top_srcdir = ../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common $(RUDIMENTSINCLUDES) `$(GTKCONFIG) --cflags`
LIBS = $(RUDIMENTSLIBS)

all: sqlr-config-gtk

clean:
	$(RM) *.o
	$(RM) sqlr-config-gtk

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

sqlr-config-gtk: configfile.C configfile.o gtkfe.C gtkfe.o
	$(CXX) $(STATICLINK) $(LDFLAGS) -o $@ gtkfe.o configfile.o `$(GTKCONFIG) --libs` $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	$(INSTALL) sqlr-config-gtk $(bindir)

uninstall:
	$(RM) $(bindir)/sqlr-config-gtk
