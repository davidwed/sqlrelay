top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $<

all: sqlrauth_userlist.$(LIBEXT) \
	sqlrauth_database.$(LIBEXT) \
	sqlrauth_sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrauth_userlist.$(LIBEXT): userlist.cpp userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

sqlrauth_database.$(LIBEXT): database.cpp database.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ database.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

sqlrauth_sqlrelay.$(LIBEXT): sqlrelay.cpp sqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelay.$(OBJ) $(LDFLAGS) $(SQLRAUTH_SQLRELAYLIBS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_database.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_sqlrelay.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_userlist.a
	$(RM) $(libexecdir)/sqlrauth_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrauth_userlist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrauth_database.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_database.a
	$(RM) $(libexecdir)/sqlrauth_database.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrauth_database.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrauth_sqlrelay.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.a
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrauth_sqlrelay.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrauth_default.*
	$(RM) $(libexecdir)/sqlrauth_userlist.*
	$(RM) $(libexecdir)/sqlrauth_database.*
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.*
