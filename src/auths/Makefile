top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(AUTHCPPFLAGS) $(COMPILE) $<

all: all-base $(AUTHALLOPTIONALTARGETS)

all-base: $(SQLR)auth_sqlrclient_connectstrings.$(LIBEXT) \
		$(SQLR)auth_sqlrclient_userlist.$(LIBEXT) \
		$(SQLR)auth_sqlrclient_database.$(LIBEXT) \
		$(SQLR)auth_sqlrclient_proxied.$(LIBEXT) \
		$(SQLR)auth_mysql_connectstrings.$(LIBEXT) \
		$(SQLR)auth_mysql_userlist.$(LIBEXT) \
		$(SQLR)auth_mysql_database.$(LIBEXT) \
		$(SQLR)auth_postgresql_connectstrings.$(LIBEXT) \
		$(SQLR)auth_postgresql_userlist.$(LIBEXT) \
		$(SQLR)auth_oracle_connectstrings.$(LIBEXT) \
		$(SQLR)auth_oracle_userlist.$(LIBEXT) \
		$(SQLR)auth_teradata_sidechannel.$(LIBEXT) \

all-sqlrelay: $(SQLR)auth_sqlrclient_sqlrelay.$(LIBEXT)

clean:
	$(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)auth_sqlrclient_connectstrings.$(LIBEXT): sqlrclient_connectstrings.cpp sqlrclient_connectstrings.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient_connectstrings.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_sqlrclient_userlist.$(LIBEXT): sqlrclient_userlist.cpp sqlrclient_userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient_userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_sqlrclient_database.$(LIBEXT): sqlrclient_database.cpp sqlrclient_database.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient_database.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_sqlrclient_proxied.$(LIBEXT): sqlrclient_proxied.cpp sqlrclient_proxied.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient_proxied.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_sqlrclient_sqlrelay.$(LIBEXT): sqlrclient_sqlrelay.cpp sqlrclient_sqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient_sqlrelay.$(OBJ) $(LDFLAGS) $(SQLRAUTH_SQLRELAYLIBS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_mysql_connectstrings.$(LIBEXT): mysql_connectstrings.cpp mysql_connectstrings.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql_connectstrings.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_mysql_userlist.$(LIBEXT): mysql_userlist.cpp mysql_userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql_userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_mysql_database.$(LIBEXT): mysql_database.cpp mysql_database.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql_database.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_postgresql_connectstrings.$(LIBEXT): postgresql_connectstrings.cpp postgresql_connectstrings.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ postgresql_connectstrings.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_postgresql_userlist.$(LIBEXT): postgresql_userlist.cpp postgresql_userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ postgresql_userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_oracle_connectstrings.$(LIBEXT): oracle_connectstrings.cpp oracle_connectstrings.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ oracle_connectstrings.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_oracle_userlist.$(LIBEXT): oracle_userlist.cpp oracle_userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ oracle_userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)auth_teradata_sidechannel.$(LIBEXT): teradata_sidechannel.cpp teradata_sidechannel.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ teradata_sidechannel.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_connectstrings.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_database.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_proxied.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_sqlrelay.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_connectstrings.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_database.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_postgresql_connectstrings.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_postgresql_userlist.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB) $(AUTHINSTALLSHAREDLIBOPTIONALTARGETS)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_connectstrings.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_connectstrings.a
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_connectstrings.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_sqlrclient_connectstrings.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_userlist.a
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_sqlrclient_userlist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_database.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_database.a
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_database.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_sqlrclient_database.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_proxied.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_proxied.a
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_proxied.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_sqlrclient_proxied.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_connectstrings.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_connectstrings.a
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_connectstrings.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_mysql_connectstrings.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_userlist.a
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_mysql_userlist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_mysql_database.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_database.a
	$(RM) $(libexecdir)/$(SQLR)auth_mysql_database.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_mysql_database.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_postgresql_connectstrings.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_postgresql_connectstrings.a
	$(RM) $(libexecdir)/$(SQLR)auth_postgresql_connectstrings.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_postgresql_connectstrings.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)auth_postgresql_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_postgresql_userlist.a
	$(RM) $(libexecdir)/$(SQLR)auth_postgresql_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_postgresql_userlist.so so $(MODULESUFFIX)

installsharedlib-sqlrelay:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)auth_sqlrclient_sqlrelay.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_sqlrelay.a
	$(RM) $(libexecdir)/$(SQLR)auth_sqlrclient_sqlrelay.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)auth_sqlrclient_sqlrelay.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)auth_default.* \
		$(libexecdir)/$(SQLR)auth_sqlrclient_connectstrings.* \
		$(libexecdir)/$(SQLR)auth_sqlrclient_userlist.* \
		$(libexecdir)/$(SQLR)auth_sqlrclient_database.* \
		$(libexecdir)/$(SQLR)auth_sqlrclient_proxied.* \
		$(libexecdir)/$(SQLR)auth_sqlrclient_sqlrelay.* \
		$(libexecdir)/$(SQLR)auth_krb_userlist.* \
		$(libexecdir)/$(SQLR)auth_tls_userlist.* \
		$(libexecdir)/$(SQLR)auth_kerberos_userlist.* \
		$(libexecdir)/$(SQLR)auth_mysql_connectstrings.* \
		$(libexecdir)/$(SQLR)auth_mysql_userlist.* \
		$(libexecdir)/$(SQLR)auth_mysql_database.* \
		$(libexecdir)/$(SQLR)auth_postgresql_connectstrings.* \
		$(libexecdir)/$(SQLR)auth_postgresql_userlist.* \
		$(libexecdir)/$(SQLR)auth_connectstrings.* \
		$(libexecdir)/$(SQLR)auth_userlist.* \
		$(libexecdir)/$(SQLR)auth_database.* \
		$(libexecdir)/$(SQLR)auth_proxied.* \
		$(libexecdir)/$(SQLR)auth_sqlrelay.* \
		$(libexecdir)/sqlrauth_default.* \
		$(libexecdir)/sqlrauth_userlist.* \
		$(libexecdir)/sqlrauth_database.* \
		$(libexecdir)/sqlrauth_proxied.* \
		$(libexecdir)/sqlrauth_krb_userlist.* \
		$(libexecdir)/sqlrauth_kerberos_userlist.* \
		$(libexecdir)/sqlrauth_sqlrelay.*
