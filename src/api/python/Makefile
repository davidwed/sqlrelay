top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(PYTHONLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PYTHONUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: CSQLRelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

CSQLRelay.$(LIBEXT): CSQLRelay.cpp CSQLRelay.lo
	$(LTLINK) $(LINK) -o $@ CSQLRelay.$(OBJ) $(LDFLAGS) $(LIBS) -rpath $(PYTHONDIR)/site-packages/SQLRelay $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)
	$(INSTALL) -m 644 PySQLRClient.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRDB.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 __init__.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(LTINSTALL) $(CP) CSQLRelay.$(LIBEXT) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(LTINSTALL) $(CP) CSQLRelay.$(LIBEXT) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(RM) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/CSQLRelay.$(LIBEXT)
	$(RM) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/CSQLRelay.a

uninstall:
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
