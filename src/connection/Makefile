include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common -I../util $(RUDIMENTSINCLUDES)

SRCS = sqlrconnection.C connection/connectioncmdline.C connection/ipc.C connection/listenercomm.C connection/scalercomm.C connection/unixsocketseqfile.C sqlrcursor.C
LOBJS = sqlrconnection.lo connection/connectioncmdline.lo connection/ipc.lo connection/listenercomm.lo connection/scalercomm.lo connection/unixsocketseqfile.lo sqlrcursor.lo
PLOBJS = sqlrconnection.p.lo connection/connectioncmdline.p.lo connection/ipc.p.lo connection/listenercomm.p.lo connection/scalercomm.p.lo connection/unixsocketseqfile.p.lo sqlrcursor.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrconnection.la libsqlrconnection_p.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) */*.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) */*.o

rebuild: clean all

libsqlrconnection.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libsqlrconnection_p.la: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(PLOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)


install:
	$(top_builddir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -s libsqlrconnection.la $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) libsqlrconnection_p.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection_p.la
