top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $<

all: all-base $(AUTHALLOPTIONALTARGETS)

all-base: $(SQLR)auth_userlist.$(LIBEXT) \
		$(SQLR)auth_database.$(LIBEXT) \
		$(SQLR)auth_krb_userlist.$(LIBEXT) \
		$(SQLR)auth_tls_userlist.$(LIBEXT)

all-sqlrelay: $(SQLR)auth_sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

$(SQLR)auth_userlist.$(LIBEXT): userlist.cpp userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_database.$(LIBEXT): database.cpp database.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ database.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_krb_userlist.$(LIBEXT): krb_userlist.cpp krb_userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ krb_userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_tls_userlist.$(LIBEXT): tls_userlist.cpp tls_userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ tls_userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_sqlrelay.$(LIBEXT): sqlrelay.cpp sqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelay.$(OBJ) $(LDFLAGS) $(SQLRAUTH_SQLRELAYLIBS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_database.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_krb_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_tls_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrelay.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB) $(AUTHINSTALLSHAREDLIBOPTIONALTARGETS)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_userlist.a
	$(RM) $(libexecdir)/$(SQLR)auth_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_userlist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_database.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_database.a
	$(RM) $(libexecdir)/$(SQLR)auth_database.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_database.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_krb_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_krb_userlist.a
	$(RM) $(libexecdir)/$(SQLR)auth_krb_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_krb userlist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_tls_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_tls_userlist.a
	$(RM) $(libexecdir)/$(SQLR)auth_tls_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_tls userlist.so so $(MODULESUFFIX)

installsharedlib-sqlrelay:
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrelay.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrelay.a
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrelay.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_sqlrelay.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)auth_default.* \
		$(libexecdir)/$(SQLR)auth_userlist.* \
		$(libexecdir)/$(SQLR)auth_database.* \
		$(libexecdir)/$(SQLR)auth_krb_userlist.* \
		$(libexecdir)/$(SQLR)auth_tls_userlist.* \
		$(libexecdir)/$(SQLR)auth_kerberos_userlist.* \
		$(libexecdir)/$(SQLR)auth_sqlrelay.* \
		$(libexecdir)/sqlrauth_default.* \
		$(libexecdir)/sqlrauth_userlist.* \
		$(libexecdir)/sqlrauth_database.* \
		$(libexecdir)/sqlrauth_krb_userlist.* \
		$(libexecdir)/sqlrauth_kerberos_userlist.* \
		$(libexecdir)/sqlrauth_sqlrelay.*
