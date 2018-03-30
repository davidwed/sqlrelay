top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)translation_normalize.$(LIBEXT) \
	$(SQLR)translation_patterns.$(LIBEXT) \

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)translation_normalize.$(LIBEXT): normalize.cpp normalize.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ normalize.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)translation_patterns.$(LIBEXT): patterns.cpp patterns.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ patterns.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)translation_normalize.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)translation_patterns.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)translation_normalize.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)translation_normalize.a
	$(RM) $(libexecdir)/$(SQLR)translation_normalize.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)translation_normalize.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)translation_patterns.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)translation_patterns.a
	$(RM) $(libexecdir)/$(SQLR)translation_patterns.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)translation_patterns.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)translation_normalize.* \
		$(libexecdir)/$(SQLR)translation_patterns.* \
		$(libexecdir)/sqlrtranslation_normalize.*
