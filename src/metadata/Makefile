top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: sqlrmetadata_default.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrmetadata_default.$(LIBEXT): default.cpp default.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ default.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrmetadata_default.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrmetadata_default.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrmetadata_default.a
	$(RM) $(libexecdir)/sqlrmetadata_default.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrmetadata_default.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrmetadata_default.*
