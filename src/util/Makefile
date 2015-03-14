top_builddir = ../..

include ../../config.mk

ifeq ($(MINGW32),yes)
DLLEXPORTS = -DLIBSQLRUTIL_EXPORTS
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(DLLEXPORTS) $(TMPDIRCPPFLAG) $(DEBUGDIRCPPFLAG) $(CONFIGFILECPPFLAG) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = $(RUDIMENTSLIBS)

SRCS = sqlrcmdline.cpp sqlrtempdir.cpp sqlrconfigfile.cpp
LOBJS = sqlrcmdline.lo sqlrtempdir.lo sqlrconfigfile.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: libsqlrutil.la

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest

libsqlrutil.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(INSTALL) sqlrelay/sqlrutil.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrutil.h
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(INSTALL) sqlrelay/private/sqlrutildll.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrutildll.h

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrutil.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrutil.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrutil.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RM) $(includedir)/sqlrelay/sqlrutil.h \
		$(includedir)/sqlrelay/private/sqlrutildll.h

uninstalldll:
	$(RM) $(bindir)/libsqlrutil.$(LIBEXT) \
		$(libdir)/libsqlrutil.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/libsqlrutil.$(LIBEXT)
