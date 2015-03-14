top_builddir = ../../../..

include ../../../../config.mk

ifeq ($(MINGW32),yes)
DLLEXPORTS = -DLIBSQLRCLIENT_EXPORTS
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(DLLEXPORTS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/api/c++/include -I$(top_builddir)/src/common $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)


SRCS = sqlrconnection.cpp sqlrcursor.cpp row.cpp
LOBJS = sqlrconnection.lo sqlrcursor.lo row.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: libsqlrclient.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest

libsqlrclient.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(cachedir)
	$(CHMOD) 777 $(cachedir)

installdll::
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrclient.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrclient.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrclient.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RMTREE) $(cachedir)

uninstalldll:
	$(LTUNINSTALL) $(RM) $(bindir)/libsqlrclient.$(LIBEXT)
	$(LTUNINSTALL) $(RM) $(libdir)/libsqlrclient.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/libsqlrclient.$(LIBEXT)
