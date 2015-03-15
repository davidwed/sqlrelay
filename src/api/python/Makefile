top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(PYTHONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(PYTHONCPPFLAGS) $(COMPILE) $<

all: CSQLRelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

CSQLRelay.$(LIBEXT): CSQLRelay.cpp CSQLRelay.lo
	$(LTLINK) $(LINK) -o $@ CSQLRelay.$(OBJ) $(LDFLAGS) $(PYTHONLIBS) $(PYTHONRPATH) $(MODLINKFLAGS)

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
