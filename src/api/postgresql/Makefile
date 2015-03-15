top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(POSTGRESQLDRLIBCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(POSTGRESQLDRLIBCPPFLAGS) $(COMPILE) $<

all: libpqsqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

libpqsqlrelay.$(LIBEXT): pqsqlrelay.cpp pqsqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ pqsqlrelay.$(OBJ) $(LDFLAGS) $(LINKFLAGS) $(POSTGRESQLDRLIBLIBS)

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
