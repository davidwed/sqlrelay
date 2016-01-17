top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)protocol_sqlrclient.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

$(SQLR)protocol_sqlrclient.$(LIBEXT): sqlrclient.cpp sqlrclient.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)protocol_sqlrclient.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)protocol_sqlrclient.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)protocol_sqlrclient.a
	$(RM) $(libexecdir)/$(SQLR)protocol_sqlrclient.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)protocol_sqlrclient.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)protocol_sqlrclient.* \
		$(libexecdir)/sqlrprotocol_sqlrclient.*
