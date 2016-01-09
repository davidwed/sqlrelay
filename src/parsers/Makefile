top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: sqlrparser_default.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrparser_default.$(LIBEXT): default.cpp default.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ default.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrparser_default.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrparser_default.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrparser_default.a
	$(RM) $(libexecdir)/sqlrparser_default.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrparser_default.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrparser_default.*
	$(RM) $(libexecdir)/sqlrmetadata_*
