top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(POSTGRESQLDRLIBCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(POSTGRESQLDRLIBCPPFLAGS) $(COMPILE) $<

all: libpq$(SQLRELAY).$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

libpq$(SQLRELAY).$(LIBEXT): pqsqlrelay.cpp pqsqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ pqsqlrelay.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(POSTGRESQLDRLIBLIBS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libpq$(SQLRELAY).$(LIBEXT) $(bindir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libpq$(SQLRELAY).$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libpq$(SQLRELAY).a
	$(RM) $(libdir)/libpq$(SQLRELAY).$(LIBEXT)

uninstall: $(UNINSTALLLIB)

uninstalldll:
	$(RM) $(bindir)/libpq$(SQLRELAY)* $(bindir)/libpqsqlrelay*

uninstalllib:
	$(RM) $(libdir)/libpq$(SQLRELAY)* $(libdir)/libpqsqlrelay*
