top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)logger_debug.$(LIBEXT) \
	$(SQLR)logger_slowqueries.$(LIBEXT) \
	$(SQLR)logger_stalecursors.$(LIBEXT) \
	$(SQLR)logger_custom_nw.$(LIBEXT) \
	$(SQLR)logger_custom_sc.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)logger_debug.$(LIBEXT): debug.cpp debug.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ debug.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)logger_slowqueries.$(LIBEXT): slowqueries.cpp slowqueries.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ slowqueries.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)logger_stalecursors.$(LIBEXT): stalecursors.cpp stalecursors.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ stalecursors.$(OBJ) $(LDFLAGS) $(SQLRLOGGER_STALECURSORSLIBS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)logger_custom_nw.$(LIBEXT): custom_nw.cpp custom_nw.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ custom_nw.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)logger_custom_sc.$(LIBEXT): custom_sc.cpp custom_sc.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ custom_sc.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)logger_debug.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)logger_slowqueries.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)logger_stalecursors.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)logger_custom_nw.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)logger_custom_sc.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)logger_debug.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)logger_debug.a
	$(RM) $(libexecdir)/$(SQLR)logger_debug.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)logger_debug.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)logger_slowqueries.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)logger_slowqueries.a
	$(RM) $(libexecdir)/$(SQLR)logger_slowqueries.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)logger_slowqueries.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)logger_stalecursors.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)logger_stalecursors.a
	$(RM) $(libexecdir)/$(SQLR)logger_stalecursors.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)logger_stalecursors.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)logger_custom_nw.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)logger_custom_nw.a
	$(RM) $(libexecdir)/$(SQLR)logger_custom_nw.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)logger_custom_nw.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)logger_custom_sc.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)logger_custom_sc.a
	$(RM) $(libexecdir)/$(SQLR)logger_custom_sc.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)logger_custom_sc.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)logger_custom_nw.* \
		$(libexecdir)/$(SQLR)logger_custom_sc.* \
		$(libexecdir)/$(SQLR)logger_debug.* \
		$(libexecdir)/$(SQLR)logger_slowqueries.* \
		$(libexecdir)/$(SQLR)logger_stalecursors.* \
		$(libexecdir)/sqlrlogger_custom_nw.* \
		$(libexecdir)/sqlrlogger_custom_sc.* \
		$(libexecdir)/sqlrlogger_debug.* \
		$(libexecdir)/sqlrlogger_slowqueries.* \
		$(libexecdir)/sqlrlogger_stalecursors.*
