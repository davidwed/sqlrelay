top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)directive_custom_wf.$(LIBEXT) \
	$(SQLR)directive_crash.$(LIBEXT) \
	$(SQLR)directive_singlestep.$(LIBEXT)

clean:
	$(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)directive_custom_wf.$(LIBEXT): custom_wf.cpp custom_wf.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ custom_wf.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)directive_crash.$(LIBEXT): crash.cpp crash.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ crash.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)directive_singlestep.$(LIBEXT): singlestep.cpp singlestep.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ singlestep.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)directive_custom_wf.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)directive_crash.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)directive_singlestep.$(LIBEXT) $(libexecdir)

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
	$(LTINSTALL) $(CP) $(SQLR)directive_singlestep.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)directive_singlestep.a
	$(RM) $(libexecdir)/$(SQLR)directive_singlestep.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)directive_singlestep.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)directive_custom_wf.*
	$(RM) $(libexecdir)/$(SQLR)directive_crash.*
	$(RM) $(libexecdir)/$(SQLR)directive_singlestep.*
