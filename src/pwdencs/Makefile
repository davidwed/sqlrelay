top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/ -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrpwdenc_rot.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrpwdenc_rot.la: rot.cpp rot.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ rot.lo $(LDFLAGS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrpwdenc_rot.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrpwdenc_rot.a
	$(RM) $(libexecdir)/sqlrpwdenc_rot.la
	$(LIBTOOL) --mode=finish $(libexecdir)

uninstall:
	$(RM) $(libexecdir)/sqlrpwdenc_*
