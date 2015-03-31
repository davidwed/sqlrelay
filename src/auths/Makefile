top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $<

all: sqlrauth_default.$(LIBEXT) sqlrauth_sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrauth_default.$(LIBEXT): default.cpp default.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ default.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

sqlrauth_sqlrelay.$(LIBEXT): sqlrelay.cpp sqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelay.$(OBJ) $(LDFLAGS) $(SQLRAUTH_SQLRELAYLIBS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_default.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_sqlrelay.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_default.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_default.a
	$(RM) $(libexecdir)/sqlrauth_default.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrauth_default.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrauth_sqlrelay.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.a
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrauth_sqlrelay.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrauth_default.*
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.*
