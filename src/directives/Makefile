top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)directive_custom_wf.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)directive_custom_wf.$(LIBEXT): custom_wf.cpp custom_wf.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ custom_wf.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)directive_custom_wf.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)directive_custom_wf.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)directive_custom_wf.a
	$(RM) $(libexecdir)/$(SQLR)directive_custom_wf.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)directive_custom_wf.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)directive_custom_wf.*
