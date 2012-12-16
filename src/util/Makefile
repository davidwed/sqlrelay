top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(TMPDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(CONFIGFILECPPFLAG) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = cmdline.cpp debugfile.cpp tempdir.cpp sqlrconfigfile.cpp authenticator.cpp sqlrpwdencs.cpp sqlrpwdenc.cpp
LOBJS = cmdline.lo debugfile.lo tempdir.lo sqlrconfigfile.lo authenticator.lo sqlrpwdencs.lo sqlrpwdenc.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrutil.la sqlrpwdenc_rot.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

libsqlrutil.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

sqlrpwdenc_rot.la: rot.cpp rot.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ rot.lo $(LDFLAGS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrutil.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrpwdenc_rot.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrpwdenc_rot.a
	$(RM) $(libexecdir)/sqlrpwdenc_rot.la
	$(LIBTOOL) --mode=finish $(libexecdir)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(INSTALL) -m 644 sqlrpwdenc.h $(includedir)/sqlrelay

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil.la
	$(RM) $(libexecdir)/sqlrpwdenc_*
	$(RM) $(libdir)/libsqlrelay_*
	$(RM) $(libdir)/libsqlrutil_p*.*
	$(RM) $(includedir)/sqlrelay/sqlrpwdenc.h
