top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: sqlrquery_sqlrcmdgstat.$(LIBEXT) \
	sqlrquery_sqlrcmdcstat.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrquery_sqlrcmdgstat.$(LIBEXT): sqlrcmdgstat.cpp sqlrcmdgstat.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrcmdgstat.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

sqlrquery_sqlrcmdcstat.$(LIBEXT): sqlrcmdcstat.cpp sqlrcmdcstat.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrcmdcstat.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrquery_sqlrcmdgstat.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrquery_sqlrcmdcstat.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrquery_sqlrcmdgstat.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.a
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrquery_sqlrcmdgstat.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrquery_sqlrcmdcstat.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.a
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrquery_sqlrcmdcstat.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.*
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.*
