top_builddir = ../../..

include ../../../config.mk

ifeq ($(DESTDIR),)
	INSTPREFIX = 
else
	INSTPREFIX = -inst-prefix-dir $(DESTDIR)
endif

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(ODBCINCLUDES) $(ICONVINCLUDES) -DSQLR_VERSION=\"$(SQLR_VERSION)\"
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(ODBCLIBS)
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

all: libsqlrodbc.la

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest

libsqlrodbc.$(LIBEXT): sqlrodbc.cpp sqlrodbc.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrodbc.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(LIBS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrodbc.$(LIBEXT) $(bindir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrodbc.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libsqlrodbc.a
	$(RM) $(libdir)/libsqlrodbc.$(LIBEXT)

uninstall: $(UNINSTALLLIB)

uninstalldll:
	$(RM) $(bindir)/libsqlrodbc*

uninstalllib:
	$(RM) $(libdir)/libsqlrodbc*
