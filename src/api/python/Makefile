top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(PYTHONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(PYTHONCPPFLAGS) $(COMPILE) $<

all: CSQLRelay.$(LIBEXT) PySQLRDB.py

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest PySQLRDB.py temp.py.in *.pyc *.ii
	$(RMTREE) .libs __pycache__

CSQLRelay.$(LIBEXT): CSQLRelay.cpp CSQLRelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ CSQLRelay.$(OBJ) $(LDFLAGS) $(PYTHONLIBS) $(MODLINKFLAGS)

PySQLRDB.py: PySQLRDB.py.in
	$(REPLACE) @IMPORTEXCEPTIONS@ "$(IMPORTEXCEPTIONS)" PySQLRDB.py.in > temp.py.in
	$(REPLACE) @EXCEPTIONSSTANDARDERROR@ "$(EXCEPTIONSSTANDARDERROR)" temp.py.in > PySQLRDB.py
	$(RM) temp.py.in

install: $(INSTALLLIB)
	$(CP) PySQLRClient.py $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay
	$(CHMOD) 644 $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay/PySQLRClient.py
	$(CP) PySQLRDB.py $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay
	$(CHMOD) 644 $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay/PySQLRDB.py
	$(CP) __init__.py $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay
	$(CHMOD) 644 $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay/__init__.py

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay
	$(LTINSTALL) $(CP) CSQLRelay.$(LIBEXT) $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay/CSQLRelay.$(PYTHONSUFFIX)

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay
	$(LTINSTALL) $(CP) CSQLRelay.$(LIBEXT) $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay
	$(RM) $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay/CSQLRelay.$(LIBEXT)
	$(RM) $(DESTDIR)$(PYTHONSITEDIR)/SQLRelay/CSQLRelay.a

uninstall:
	$(RMTREE) $(PYTHONSITEDIR)/SQLRelay
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
	$(RMTREE) $(PYTHONDIR)/dist-packages/SQLRelay
