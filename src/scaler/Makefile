include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) $(TMPDIRCPPFLAG) $(CONFIGFILECPPFLAG) -I./ -I../../ -I../common -I../util $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) -L../util -lsqlrutil $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = scaler.C main.C
LOBJS = scaler.lo main.lo
PLOBJS = scaler.p.lo main.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-scaler sqlr-scaler-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-scaler
	$(LIBTOOL) --mode=clean $(RM) sqlr-scaler-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-scaler: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS)

sqlr-scaler-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) $(LIBS)

install:
	$(top_builddir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-scaler $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) sqlr-scaler-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-scaler
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-scaler-debug
