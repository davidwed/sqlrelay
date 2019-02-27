top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)trigger_replay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)trigger_replay.$(LIBEXT): replay.cpp replay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ replay.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)trigger_replay.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)trigger_replay.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)trigger_replay.a
	$(RM) $(libexecdir)/$(SQLR)trigger_replay.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)trigger_replay.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)trigger_replay.*
	$(RM) $(libexecdir)/$(SQLR)trigger_deadlock_replay.*
