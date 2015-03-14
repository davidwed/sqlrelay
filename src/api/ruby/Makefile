top_builddir = ../../..

include ../../../config.mk

RUBYCFLAGS = $(shell LANG=POSIX $(RUBY) getcflags.rb | sed -e "s|-x.* | |g" -e "s|-belf||g" -e "s|-mtune=.* | |g" | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )

ifeq ($(OVERRIDERUBYSITEARCHDIR),)
RUBYSITEARCHDIR = $(shell LANG=POSIX $(RUBY) getsitearchdir.rb | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )
else
RUBYSITEARCHDIR = $(OVERRIDERUBYSITEARCHDIR)
endif

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(RUBYCFLAGS) -I./ -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(RUBYLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(RUBYUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	cp rubyincludes1.h rubyincludes.h; \
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@ 2> errors1; \
	if ( test ! -r sqlrelay.lo ); then \
		cp rubyincludes2.h rubyincludes.h; \
		$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@ 2> errors2; \
		if ( test ! -r sqlrelay.lo ); then \
			if ( test -s errors1 ); then \
				echo "first try errors : "; cat errors1; echo; \
			fi; \
			if ( test -s errors2 ); then \
				echo "second try errors : "; cat errors2; echo; \
			fi; \
			exit 1; \
		else \
			cat errors2; \
		fi; \
	else \
		cat errors1; \
	fi; \
	$(RM) errors1 errors2 rubyincludes.h


all: sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.so
	$(RMTREE) .libs

sqlrelay.$(LIBEXT): sqlrelay.cpp sqlrelay.lo
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelay.$(OBJ) $(LDFLAGS) $(LIBS) -rpath $(RUBYSITEARCHDIR) $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(RUBYSITEARCHDIR)
	$(LTINSTALL) $(CP) sqlrelay.la $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.$(MODULESUFFIX)

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(RUBYSITEARCHDIR)
	$(LTINSTALL) $(CP) sqlrelay.la $(DESTDIR)$(RUBYSITEARCHDIR)
	$(RM) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.a
	$(RM) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.la
	$(MODULERENAME) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.$(MODULESUFFIX)
