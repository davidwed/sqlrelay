top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $<

all: sqlrconfig_xml.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconfig_xml.$(LIBEXT): xml.cpp xml.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ xml.$(OBJ) $(LDFLAGS) $(CONFIGPLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconfig_xml.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconfig_xml.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconfig_xml.a
	$(RM) $(libexecdir)/sqlrconfig_xml.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconfig_xml.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconfig_xml.*
