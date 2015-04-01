top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: sqlrprotocol_sqlrclient.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrprotocol_sqlrclient.$(LIBEXT): sqlrclient.cpp sqlrclient.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrprotocol_sqlrclient.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrprotocol_sqlrclient.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.a
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrprotocol_sqlrclient.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.*
