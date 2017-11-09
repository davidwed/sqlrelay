top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)router_regex.$(LIBEXT) \
	$(SQLR)router_userlist.$(LIBEXT) \
	$(SQLR)router_clientiplist.$(LIBEXT) \
	$(SQLR)router_clientinfolist.$(LIBEXT) \
	$(SQLR)router_usedatabase.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)router_regex.$(LIBEXT): regex.cpp regex.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ regex.$(OBJ) $(LDFLAGS) $(ROUTERPLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)router_userlist.$(LIBEXT): userlist.cpp userlist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ userlist.$(OBJ) $(LDFLAGS) $(ROUTERPLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)router_clientiplist.$(LIBEXT): clientiplist.cpp clientiplist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ clientiplist.$(OBJ) $(LDFLAGS) $(ROUTERPLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)router_clientinfolist.$(LIBEXT): clientinfolist.cpp clientinfolist.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ clientinfolist.$(OBJ) $(LDFLAGS) $(ROUTERPLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)router_usedatabase.$(LIBEXT): usedatabase.cpp usedatabase.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ usedatabase.$(OBJ) $(LDFLAGS) $(ROUTERPLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)router_regex.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)router_userlist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)router_clientiplist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)router_clientinfolist.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)router_usedatabase.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)router_regex.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)router_regex.a
	$(RM) $(libexecdir)/$(SQLR)router_regex.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)router_regex.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)router_userlist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)router_userlist.a
	$(RM) $(libexecdir)/$(SQLR)router_userlist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)router_userlist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)router_clientiplist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)router_clientiplist.a
	$(RM) $(libexecdir)/$(SQLR)router_clientiplist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)router_clientiplist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)router_clientinfolist.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)router_clientinfolist.a
	$(RM) $(libexecdir)/$(SQLR)router_clientinfolist.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)router_clientinfolist.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)router_usedatabase.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)router_usedatabase.a
	$(RM) $(libexecdir)/$(SQLR)router_usedatabase.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)router_usedatabase.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)router_regex.*
	$(RM) $(libexecdir)/$(SQLR)router_userlist.*
	$(RM) $(libexecdir)/$(SQLR)router_clientiplist.*
	$(RM) $(libexecdir)/$(SQLR)router_clientinfolist.*
	$(RM) $(libexecdir)/$(SQLR)router_usedatabase.*
