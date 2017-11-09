top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)resultsettranslation_reformatdatetime.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)resultsettranslation_reformatdatetime.$(LIBEXT): reformatdatetime.cpp reformatdatetime.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ reformatdatetime.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)resultsettranslation_reformatdatetime.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)resultsettranslation_reformatdatetime.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)resultsettranslation_reformatdatetime.a
	$(RM) $(libexecdir)/$(SQLR)resultsettranslation_reformatdatetime.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)resultsettranslation_reformatdatetime.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)resultsettranslation_reformatdatetime.* \
		$(libexecdir)/sqlrresultsettranslation_reformatdatetime.*
