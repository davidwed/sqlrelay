top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)directive_custom_wf.$(LIBEXT) \
	$(SQLR)directive_crash.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)directive_custom_wf.$(LIBEXT): custom_wf.cpp custom_wf.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ custom_wf.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)directive_crash.$(LIBEXT): crash.cpp crash.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ crash.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)directive_custom_wf.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)directive_crash.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)directive_custom_wf.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)directive_custom_wf.a
	$(RM) $(libexecdir)/$(SQLR)directive_custom_wf.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)directive_custom_wf.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)directive_crash.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)directive_crash.a
	$(RM) $(libexecdir)/$(SQLR)directive_crash.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)directive_crash.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)directive_custom_wf.*
	$(RM) $(libexecdir)/$(SQLR)directive_crash.*
