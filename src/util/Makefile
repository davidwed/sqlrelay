include ../../config.mk

top_builddir = ../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) $(TMPDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(CONFIGFILECPPFLAG) -I./ -I../../ -I../common -I../libsqlrutil $(RUDIMENTSINCLUDES)

SRCS = cmdline.C debugfile.C tempdir.C sqlrconfigfile.C authenticator.C
LOBJS = cmdline.lo debugfile.lo tempdir.lo sqlrconfigfile.lo authenticator.lo
PLOBJS = cmdline.p.lo debugfile.p.lo tempdir.p.lo sqlrconfigfile.p.lo authenticator.p.lo

.SUFFIXES: .lo .p.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrutil.la libsqlrutil_p.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

libsqlrutil.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

libsqlrutil_p.la: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(PLOBJS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(top_builddir)/mkinstalldirs $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) -s libsqlrutil.la $(libdir)
	$(LIBTOOL) --mode=install $(INSTALL) libsqlrutil_p.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil_p.la
