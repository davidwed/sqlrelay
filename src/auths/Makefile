top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver $(RUDIMENTSLIBS) $(EXTRALIBS)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) /I$(top_builddir)/src/util /I$(top_builddir)/src/server/include /I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(COMPILE) $<

all: sqlrauth_default.$(LIBEXT) sqlrauth_sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrauth_default.$(LIBEXT): default.cpp default.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ default.$(OBJ) $(LDFLAGS) $(LIBS) $(MODLINKFLAGS)

sqlrauth_sqlrelay.$(LIBEXT): sqlrelay.cpp sqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelay.$(OBJ) $(LDFLAGS) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(LIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_default.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_sqlrelay.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrauth_default.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_default.a
	$(RM) $(libexecdir)/sqlrauth_default.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrauth_default.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrauth_sqlrelay.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.a
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrauth_sqlrelay.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrauth_default.*
	$(RM) $(libexecdir)/sqlrauth_sqlrelay.*
