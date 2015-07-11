top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: sqlrfilter_regex.$(LIBEXT) \
	sqlrfilter_string.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrfilter_regex.$(LIBEXT): regex.cpp regex.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ regex.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

sqlrfilter_string.$(LIBEXT): string.cpp string.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ string.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrfilter_regex.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrfilter_string.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrfilter_regex.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrfilter_regex.a
	$(RM) $(libexecdir)/sqlrfilter_regex.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrfilter_regex.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrfilter_string.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrfilter_string.a
	$(RM) $(libexecdir)/sqlrfilter_string.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrfilter_string.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrfilter_regex.*
	$(RM) $(libexecdir)/sqlrfilter_string.*
