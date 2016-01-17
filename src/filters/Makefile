top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)filter_regex.$(LIBEXT) \
	$(SQLR)filter_string.$(LIBEXT) \
	$(SQLR)filter_patterns.$(LIBEXT) \

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

$(SQLR)filter_regex.$(LIBEXT): regex.cpp regex.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ regex.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)filter_string.$(LIBEXT): string.cpp string.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ string.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)filter_patterns.$(LIBEXT): patterns.cpp patterns.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ patterns.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)filter_regex.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)filter_string.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)filter_patterns.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)filter_regex.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)filter_regex.a
	$(RM) $(libexecdir)/$(SQLR)filter_regex.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)filter_regex.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)filter_string.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)filter_string.a
	$(RM) $(libexecdir)/$(SQLR)filter_string.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)filter_string.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)filter_patterns.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)filter_patterns.a
	$(RM) $(libexecdir)/$(SQLR)filter_patterns.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)filter_patterns.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)filter_regex.* \
		$(libexecdir)/$(SQLR)filter_string.* \
		$(libexecdir)/$(SQLR)filter_patterns.* \
		$(libexecdir)/sqlrfilter_regex.* \
		$(libexecdir)/sqlrfilter_string.* \
		$(libexecdir)/sqlrfilter_patterns.*
