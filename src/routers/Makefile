top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)router_default.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

$(SQLR)router_default.$(LIBEXT): default.cpp default.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ default.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)router_default.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)router_default.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)router_default.a
	$(RM) $(libexecdir)/$(SQLR)router_default.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)router_default.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)router_default.*
