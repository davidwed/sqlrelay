top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)errortranslation_patterns.$(LIBEXT) \
	$(SQLR)errortranslation_renumber.$(LIBEXT)

clean:
	$(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)errortranslation_patterns.$(LIBEXT): patterns.cpp patterns.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ patterns.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)errortranslation_renumber.$(LIBEXT): renumber.cpp renumber.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ renumber.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)errortranslation_patterns.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)errortranslation_renumber.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)errortranslation_patterns.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)errortranslation_patterns.a
	$(RM) $(libexecdir)/$(SQLR)errortranslation_patterns.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)errortranslation_patterns.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)errortranslation_renumber.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)errortranslation_renumber.a
	$(RM) $(libexecdir)/$(SQLR)errortranslation_renumber.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)errortranslation_renumber.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)errortranslation_patterns.* \
		$(libexecdir)/$(SQLR)errortranslation_renumber.*
