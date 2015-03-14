top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver $(RUDIMENTSLIBS) $(EXTRALIBS)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) /I$(top_builddir)/src/util /I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(COMPILE) $<

all: sqlrpwdenc_rot.$(LIBEXT) \
	sqlrpwdenc_md5.$(LIBEXT) \
	sqlrpwdenc_crypt.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrpwdenc_rot.$(LIBEXT): rot.cpp rot.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ rot.$(OBJ) $(LDFLAGS) $(LIBS) $(MODLINKFLAGS)

sqlrpwdenc_md5.$(LIBEXT): md5.cpp md5.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ md5.$(OBJ) $(LDFLAGS) $(LIBS) $(MODLINKFLAGS)

sqlrpwdenc_crypt.$(LIBEXT): crypt.cpp crypt.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ crypt.$(OBJ) $(LDFLAGS) $(LIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrpwdenc_rot.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrpwdenc_md5.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrpwdenc_crypt.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrpwdenc_rot.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrpwdenc_rot.a
	$(RM) $(libexecdir)/sqlrpwdenc_rot.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrpwdenc_rot.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrpwdenc_md5.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrpwdenc_md5.a
	$(RM) $(libexecdir)/sqlrpwdenc_md5.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrpwdenc_md5.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrpwdenc_crypt.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrpwdenc_crypt.a
	$(RM) $(libexecdir)/sqlrpwdenc_crypt.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrpwdenc_crypt.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrpwdenc_rot.*
	$(RM) $(libexecdir)/sqlrpwdenc_md5.*
	$(RM) $(libexecdir)/sqlrpwdenc_crypt.*
