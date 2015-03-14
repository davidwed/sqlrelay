top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) /I$(top_builddir)/ /I$(top_builddir)/src/server/include /I$(top_builddir)/src/common /I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) $(COMPILE) $<

all: sqlrparser_default.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrparser_default.$(LIBEXT): default.cpp default.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) -o $@ default.$(OBJ) $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrparser_default.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrparser_default.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrparser_default.a
	$(RM) $(libexecdir)/sqlrparser_default.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrparser_default.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrparser_default.*
