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

CSQLRelay.$(LIBEXT): CSQLRelay.cpp CSQLRelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ CSQLRelay.$(OBJ) $(LDFLAGS) $(PYTHONLIBS) $(PYTHONRPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)
	$(CP) PySQLRClient.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(CHMOD) 644 $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/PySQLRClient.py
	$(CP) PySQLRDB.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(CHMOD) 644 $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/PySQLRDB.py
	$(CP) __init__.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(CHMOD) 644 $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/__init__.py

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(LTINSTALL) $(CP) CSQLRelay.$(LIBEXT) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/CSQLRelay.$(PYTHONSUFFIX)

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(LTINSTALL) $(CP) CSQLRelay.$(LIBEXT) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(RM) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/CSQLRelay.$(LIBEXT)
	$(RM) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/CSQLRelay.a

uninstall:
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
