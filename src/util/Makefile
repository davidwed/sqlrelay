top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

ifeq ($(MINGW32),yes)
DLLEXPORTS = -DLIBSQLRUTIL_EXPORTS
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(DLLEXPORTS) $(TMPDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(CONFIGFILECPPFLAG) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = $(RUDIMENTSLIBS)

SRCS = cmdline.cpp tempdir.cpp sqlrconfigfile.cpp
LOBJS = cmdline.lo tempdir.lo sqlrconfigfile.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrutil.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

libsqlrutil.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrutil.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil.la
	$(RM) $(libdir)/libsqlrelay_*
	$(RM) $(libdir)/libsqlrutil_p*.*
