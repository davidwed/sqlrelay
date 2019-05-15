top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)pwdenc_rot.$(LIBEXT) \
	$(SQLR)pwdenc_md5.$(LIBEXT) \
	$(SQLR)pwdenc_sha1.$(LIBEXT) \
	$(SQLR)pwdenc_sha256.$(LIBEXT) \
	$(SQLR)pwdenc_crypt.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)pwdenc_rot.$(LIBEXT): rot.cpp rot.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ rot.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)pwdenc_md5.$(LIBEXT): md5.cpp md5.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ md5.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)pwdenc_sha1.$(LIBEXT): sha1.cpp sha1.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sha1.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)pwdenc_sha256.$(LIBEXT): sha256.cpp sha256.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sha256.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)pwdenc_crypt.$(LIBEXT): crypt.cpp crypt.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ crypt.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_rot.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_md5.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_sha1.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_sha256.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_crypt.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_rot.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)pwdenc_rot.a
	$(RM) $(libexecdir)/$(SQLR)pwdenc_rot.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)pwdenc_rot.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_md5.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)pwdenc_md5.a
	$(RM) $(libexecdir)/$(SQLR)pwdenc_md5.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)pwdenc_md5.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_sha1.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)pwdenc_sha1.a
	$(RM) $(libexecdir)/$(SQLR)pwdenc_sha1.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)pwdenc_sha1.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_sha256.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)pwdenc_sha256.a
	$(RM) $(libexecdir)/$(SQLR)pwdenc_sha256.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)pwdenc_sha256.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)pwdenc_crypt.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)pwdenc_crypt.a
	$(RM) $(libexecdir)/$(SQLR)pwdenc_crypt.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)pwdenc_crypt.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)pwdenc_rot.* \
		$(libexecdir)/$(SQLR)pwdenc_md5.* \
		$(libexecdir)/$(SQLR)pwdenc_crypt.* \
		$(libexecdir)/sqlrpwdenc_rot.* \
		$(libexecdir)/sqlrpwdenc_md5.* \
		$(libexecdir)/sqlrpwdenc_sha1.* \
		$(libexecdir)/sqlrpwdenc_sha256.* \
		$(libexecdir)/sqlrpwdenc_crypt.*
