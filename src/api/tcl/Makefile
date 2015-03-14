top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(TCLINCLUDE) -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(TCLLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(TCLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<
	

all: sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrelay.$(LIBEXT): sqlrelayCmd.cpp sqlrelayCmd.lo
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelayCmd.$(OBJ) $(LDFLAGS) $(LIBS) -rpath $(TCLLIBSPATH)/sqlrelay $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)
	$(CP) pkgIndex.tcl $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(CHMOD) 644 $(DESTDIR)$(TCLLIBSPATH)/sqlrelay/pkgIndex.tcl

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(LTINSTALL) $(CP) sqlrelay.$(LIBEXT) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(LTINSTALL) $(CP) sqlrelay.$(LIBEXT) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(RM) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay/sqlrelay.$(LIBEXT)
	$(RM) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay/sqlrelay.a

uninstall:
	$(RMTREE) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
