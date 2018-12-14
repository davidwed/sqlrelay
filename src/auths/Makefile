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
		$(SQLR)auth_proxied.$(LIBEXT) \
		$(SQLR)auth_mysql_userlist.$(LIBEXT) \
		$(SQLR)auth_mysql_database.$(LIBEXT)

all-sqlrelay: $(SQLR)auth_sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)auth_userlist.$(LIBEXT): userlist.cpp userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_database.$(LIBEXT): database.cpp database.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ database.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_proxied.$(LIBEXT): proxied.cpp proxied.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ proxied.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_sqlrelay.$(LIBEXT): sqlrelay.cpp sqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelay.$(OBJ) $(LDFLAGS) $(SQLRAUTH_SQLRELAYLIBS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_mysql_userlist.$(LIBEXT): mysql_userlist.cpp mysql_userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql_userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_mysql_database.$(LIBEXT): mysql_database.cpp mysql_database.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql_database.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_database.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_proxied.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrelay.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_database.$(LIBEXT) $(libexecdir)

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
	$(LTINSTALL) $(CP) $(SQLR)auth_proxied.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_proxied.a
	$(RM) $(libexecdir)/$(SQLR)auth_proxied.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_proxied.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_userlist.a
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_mysql_userlist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_database.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_database.a
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_database.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_mysql_database.so so $(MODULESUFFIX)

installsharedlib-sqlrelay:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrelay.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrelay.a
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrelay.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_sqlrelay.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)auth_default.* \
		$(libexecdir)/$(SQLR)auth_userlist.* \
		$(libexecdir)/$(SQLR)auth_database.* \
		$(libexecdir)/$(SQLR)auth_proxied.* \
		$(libexecdir)/$(SQLR)auth_krb_userlist.* \
		$(libexecdir)/$(SQLR)auth_tls_userlist.* \
		$(libexecdir)/$(SQLR)auth_kerberos_userlist.* \
		$(libexecdir)/$(SQLR)auth_sqlrelay.* \
		$(libexecdir)/$(SQLR)auth_mysql_userlist.* \
		$(libexecdir)/$(SQLR)auth_mysql_database.* \
		$(libexecdir)/sqlrauth_default.* \
		$(libexecdir)/sqlrauth_userlist.* \
		$(libexecdir)/sqlrauth_database.* \
		$(libexecdir)/sqlrauth_proxied.* \
		$(libexecdir)/sqlrauth_krb_userlist.* \
		$(libexecdir)/sqlrauth_kerberos_userlist.* \
		$(libexecdir)/sqlrauth_sqlrelay.*
