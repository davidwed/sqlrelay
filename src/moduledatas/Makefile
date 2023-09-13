top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(PLUGINCPPFLAGS) $(COMPILE) $<

all: $(SQLR)moduledata_tag.$(LIBEXT)

clean:
	$(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

$(SQLR)moduledata_tag.$(LIBEXT): tag.cpp tag.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ tag.$(OBJ) $(LDFLAGS) $(PLUGINLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)moduledata_tag.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) $(SQLR)moduledata_tag.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/$(SQLR)moduledata_tag.a
	$(RM) $(libexecdir)/$(SQLR)moduledata_tag.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/$(SQLR)moduledata_tag.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/$(SQLR)moduledata_tag.*
