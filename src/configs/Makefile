top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $<

all: $(SQLR)config_xmldom.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)config_xmldom.$(LIBEXT): xmldom.cpp xmldom.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ xmldom.$(OBJ) $(LDFLAGS) $(CONFIGPLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)config_xmldom.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(MODULERENAME) $(libexecdir)/$(SQLR)config_xml.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)config_xmldom.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)config_xmldom.a
	$(RM) $(libexecdir)/$(SQLR)config_xmldom.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)config_xmldom.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)config_xmldom.* $(libexecdir)/sqlrconfig_xmldom.*
	$(RM) $(libexecdir)/$(SQLR)config_xml.* $(libexecdir)/sqlrconfig_xml.*
