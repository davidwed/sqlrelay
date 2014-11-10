top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

ifeq ($(MINGW32),yes)
DLLEXPORTS = -DLIBSQLRUTIL_EXPORTS
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(DLLEXPORTS) $(TMPDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(CONFIGFILECPPFLAG) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = $(RUDIMENTSLIBS)

SRCS = sqlrcmdline.cpp sqlrtempdir.cpp sqlrconfigfile.cpp
LOBJS = sqlrcmdline.lo sqlrtempdir.lo sqlrconfigfile.lo

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
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(INSTALL) -m 644 sqlrelay/sqlrutil.h $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(INSTALL) -m 644 sqlrelay/private/sqlrutildll.h $(includedir)/sqlrelay/private
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrutil.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrutil.la
	$(RM) $(includedir)/sqlrelay/sqlrutil.h
	$(RM) $(includedir)/sqlrelay/private/sqlrutildll.h
	$(RM) $(libdir)/libsqlrelay_*
	$(RM) $(libdir)/libsqlrutil_p*.*
