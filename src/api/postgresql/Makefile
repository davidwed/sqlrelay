top_builddir = ../../..

include ../../../config.mk

ifeq ($(DESTDIR),)
	INSTPREFIX = 
else
	INSTPREFIX = -inst-prefix-dir $(DESTDIR)
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS)
ifneq ($(strip $(libdir)),)
ifeq ($(PSQLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: libpqsqlrelay.la

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest

libpqsqlrelay.$(LIBEXT): pqsqlrelay.cpp pqsqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ pqsqlrelay.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libpqsqlrelay.$(LIBEXT) $(bindir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libpqsqlrelay.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libpqsqlrelay.a
	$(RM) $(libdir)/libpqsqlrelay.$(LIBEXT)

uninstall: $(UNINSTALLLIB)

uninstalldll:
	$(RM) $(bindir)/libpqsqlrelay*

uninstalllib:
	$(RM) $(libdir)/libpqsqlrelay*
