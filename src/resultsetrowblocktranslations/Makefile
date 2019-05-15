top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)resultsetrowblocktranslation_test.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)resultsetrowblocktranslation_test.$(LIBEXT): test.cpp test.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ test.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)resultsetrowblocktranslation_test.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)resultsetrowblocktranslation_test.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)resultsetrowblocktranslation_test.a
	$(RM) $(libexecdir)/$(SQLR)resultsetrowblocktranslation_test.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)resultsetrowblocktranslation_test.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)resultsetrowblocktranslation_test.* \
		$(libexecdir)/sqlrresultsetrowblocktranslation_test.*
