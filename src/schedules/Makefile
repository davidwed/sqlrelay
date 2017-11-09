top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)schedule_cron_userlist.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)schedule_cron_userlist.$(LIBEXT): cron_userlist.cpp cron_userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ cron_userlist.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)schedule_cron_userlist.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)schedule_cron_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)schedule_cron_userlist.a
	$(RM) $(libexecdir)/$(SQLR)schedule_cron_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)schedule_cron_userlist.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)schedule_cron_userlist.*
