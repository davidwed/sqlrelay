top_builddir = ../../../..

include ../../../../config.mk

ifeq ($(MINGW32),yes)
DLLEXPORTS = -DLIBSQLRCLIENTWRAPPER_EXPORTS
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(WNOMISMATCHEDTAGS) $(DLLEXPORTS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c/include -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(libdir)),)
ifeq ($(CUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = sqlrclientwrapper.cpp
LOBJS = sqlrclientwrapper.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: libsqlrclientwrapper.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest

libsqlrclientwrapper.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(cachedir)
	$(CHMOD) 777 $(cachedir)

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrclientwrapper.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrclientwrapper.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrclientwrapper.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RMTREE) $(cachedir)

uninstalldll:
	$(RM) $(bindir)/libsqlrclientwrapper.$(LIBEXT) \
		$(libdir)/libsqlrclientwrapper.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/libsqlrclientwrapper.$(LIBEXT)
