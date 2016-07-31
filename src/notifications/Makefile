top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)notification_event.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

$(SQLR)notification_event.$(LIBEXT): event.cpp event.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ event.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)notification_event.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)notification_event.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)notification_event.a
	$(RM) $(libexecdir)/$(SQLR)notification_event.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)notification_event.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)notification_event.*
