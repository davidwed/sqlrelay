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

all: libsqlrutil.la libsqlrelay_sqlrpwdenc_rot.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

libsqlrutil.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

libsqlrelay_sqlrpwdenc_rot.la: rot.cpp rot.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ rot.lo $(LDFLAGS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrutil.la $(libdir)
	$(LTINSTALL) libsqlrelay_sqlrpwdenc_rot.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqlrpwdenc_rot.a
	$(LIBTOOL) --mode=finish $(libdir)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(INSTALL) -m 644 sqlrpwdenc.h $(includedir)/sqlrelay

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil_p.la
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqlrpwdenc_rot.la
	$(RM) $(includedir)/sqlrelay/sqlrpwdenc.h
