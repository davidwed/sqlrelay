top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.odj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) /I$(top_builddir)/ /I$(top_builddir)/src/server/include /I$(top_builddir)/src/common /I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(COMPILE) $<

all: sqlrprotocol_sqlrclient.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrprotocol_sqlrclient.$(LIBEXT): sqlrclient.cpp sqlrclient.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrclient.$(OBJ) $(LDFLAGS) $(LIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrprotocol_sqlrclient.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrprotocol_sqlrclient.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.a
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrprotocol_sqlrclient.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrprotocol_sqlrclient.*
