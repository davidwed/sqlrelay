top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)notification_events.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)notification_events.$(LIBEXT): events.cpp events.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ events.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)notification_events.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)notification_events.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)notification_events.a
	$(RM) $(libexecdir)/$(SQLR)notification_events.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)notification_events.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)notification_events.*
