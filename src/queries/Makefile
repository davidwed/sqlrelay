top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) /I./ /I$(top_builddir)/ /I$(top_builddir)/src/server/include /I$(top_builddir)/src/common /I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(COMPILE) $<

all: sqlrquery_sqlrcmdgstat.$(LIBEXT) \
	sqlrquery_sqlrcmdcstat.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrquery_sqlrcmdgstat.$(LIBEXT): sqlrcmdgstat.cpp sqlrcmdgstat.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrcmdgstat.$(OBJ) $(LDFLAGS) $(LIBS) $(MODLINKFLAGS)

sqlrquery_sqlrcmdcstat.$(LIBEXT): sqlrcmdcstat.cpp sqlrcmdcstat.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrcmdcstat.$(OBJ) $(LDFLAGS) $(LIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrquery_sqlrcmdgstat.$(LIBEXT) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrquery_sqlrcmdcstat.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrquery_sqlrcmdgstat.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.a
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrquery_sqlrcmdgstat.so so $(MODULESUFFIX)
	$(LTINSTALL) $(CP) sqlrquery_sqlrcmdcstat.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.a
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrquery_sqlrcmdcstat.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdgstat.*
	$(RM) $(libexecdir)/sqlrquery_sqlrcmdcstat.*
