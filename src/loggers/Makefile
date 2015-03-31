top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(LOGGERCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(LOGGERCPPFLAGS) $(COMPILE) $<

all: sqlrlogger_debug.$(LIBEXT) \
	sqlrlogger_slowqueries.$(LIBEXT) \
	sqlrlogger_custom_nw.$(LIBEXT) \
	sqlrlogger_custom_sc.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrlogger_debug.$(LIBEXT): debug.cpp debug.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ debug.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

sqlrlogger_slowqueries.$(LIBEXT): slowqueries.cpp slowqueries.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ slowqueries.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

sqlrlogger_custom_nw.$(LIBEXT): custom_nw.cpp custom_nw.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ custom_nw.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

sqlrlogger_custom_sc.$(LIBEXT): custom_sc.cpp custom_sc.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ custom_sc.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrlogger_debug.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrlogger_slowqueries.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrlogger_custom_nw.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrlogger_custom_sc.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrlogger_debug.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrlogger_debug.a
	$(RM) $(libexecdir)/sqlrlogger_debug.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrlogger_debug.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrlogger_slowqueries.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrlogger_slowqueries.a
	$(RM) $(libexecdir)/sqlrlogger_slowqueries.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrlogger_slowqueries.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrlogger_custom_nw.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrlogger_custom_nw.a
	$(RM) $(libexecdir)/sqlrlogger_custom_nw.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrlogger_custom_nw.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrlogger_custom_sc.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrlogger_custom_sc.a
	$(RM) $(libexecdir)/sqlrlogger_custom_sc.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrlogger_custom_sc.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrlogger_custom_nw.*
	$(RM) $(libexecdir)/sqlrlogger_custom_sc.*
	$(RM) $(libexecdir)/sqlrlogger_debug.*
	$(RM) $(libexecdir)/sqlrlogger_slowqueries.*
