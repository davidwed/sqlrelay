top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)schedule_cron.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

$(SQLR)schedule_cron.$(LIBEXT): cron.cpp cron.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ cron.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)schedule_cron.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)schedule_cron.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)schedule_cron.a
	$(RM) $(libexecdir)/$(SQLR)schedule_cron.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)schedule_cron.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)schedule_cron.*
