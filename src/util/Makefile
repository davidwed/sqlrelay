top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(TMPDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(CONFIGFILECPPFLAG) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) -DLIBDIR=\"$(libdir)\"
LIBS = $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = cmdline.cpp debugfile.cpp tempdir.cpp sqlrconfigfile.cpp authenticator.cpp sqlrpwdencs.cpp sqlrpwdenc.cpp
LOBJS = cmdline.lo debugfile.lo tempdir.lo sqlrconfigfile.lo authenticator.lo sqlrpwdencs.lo sqlrpwdenc.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrutil.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

libsqlrutil.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrutil.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil_p.la
