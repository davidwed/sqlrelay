top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)protocol_sqlrclient.$(LIBEXT) \
	$(SQLR)protocol_mysql.$(LIBEXT) \

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)protocol_sqlrclient.$(LIBEXT): sqlrclient.cpp sqlrclient.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)protocol_mysql.$(LIBEXT): mysql.cpp mysql.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)protocol_sqlrclient.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)protocol_mysql.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)protocol_sqlrclient.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)protocol_sqlrclient.a
	$(RM) $(libexecdir)/$(SQLR)protocol_sqlrclient.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)protocol_sqlrclient.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) $(SQLR)protocol_mysql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)protocol_mysql.a
	$(RM) $(libexecdir)/$(SQLR)protocol_mysql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)protocol_mysql.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)protocol_sqlrclient.* \
		$(libexecdir)/sqlrprotocol_sqlrclient.* \
		$(libexecdir)/sqlrprotocol_mysql.*
