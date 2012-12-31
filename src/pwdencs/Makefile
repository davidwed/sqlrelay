top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server -lsqlrserver $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/src/server $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrpwdenc_rot.la \
	sqlrpwdenc_md5.la \
	sqlrpwdenc_crypt.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrpwdenc_rot.la: rot.cpp rot.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ rot.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrpwdenc_md5.la: md5.cpp md5.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ md5.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrpwdenc_crypt.la: crypt.cpp crypt.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ crypt.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrpwdenc_rot.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrpwdenc_rot.a
	$(RM) $(libexecdir)/sqlrpwdenc_rot.la
	$(MODULERENAME) $(libexecdir)/sqlrpwdenc_rot.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrpwdenc_md5.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrpwdenc_md5.a
	$(RM) $(libexecdir)/sqlrpwdenc_md5.la
	$(MODULERENAME) $(libexecdir)/sqlrpwdenc_md5.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrpwdenc_crypt.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrpwdenc_crypt.a
	$(RM) $(libexecdir)/sqlrpwdenc_crypt.la
	$(MODULERENAME) $(libexecdir)/sqlrpwdenc_crypt.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrpwdenc_*
