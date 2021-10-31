top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)protocol_sqlrclient.$(LIBEXT) \
	$(SQLR)protocol_mysql.$(LIBEXT) \
	$(SQLR)protocol_postgresql.$(LIBEXT) \
	$(SQLR)protocol_tds.$(LIBEXT) \
	$(SQLR)protocol_oracle.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)protocol_sqlrclient.$(LIBEXT): sqlrclient.cpp sqlrclient.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)protocol_mysql.$(LIBEXT): mysql.cpp mysql.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mysql.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)protocol_postgresql.$(LIBEXT): postgresql.cpp postgresql.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ postgresql.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)protocol_tds.$(LIBEXT): tds.cpp tds.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ tds.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

$(SQLR)protocol_oracle.$(LIBEXT): oracle.cpp oracle.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ oracle.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)protocol_sqlrclient.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)protocol_mysql.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)protocol_postgresql.$(LIBEXT) $(libexecdir)

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
	$(LTINSTALL) $(CP) $(SQLR)protocol_postgresql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)protocol_postgresql.a
	$(RM) $(libexecdir)/$(SQLR)protocol_postgresql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)protocol_postgresql.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)protocol_sqlrclient.* \
		$(libexecdir)/sqlrprotocol_sqlrclient.* \
		$(libexecdir)/sqlrprotocol_mysql.* \
		$(libexecdir)/sqlrprotocol_postgresql.*
