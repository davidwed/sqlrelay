top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	cp rubyincludes1.h rubyincludes.h; \
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(RUBYCPPFLAGS) $(COMPILE) $< $(OUT)$@ 2> errors1; \
	if ( test ! -r sqlrelay.lo ); then \
		cp rubyincludes2.h rubyincludes.h; \
		$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(RUBYCPPFLAGS) $(COMPILE) $< $(OUT)$@ 2> errors2; \
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
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelay.$(OBJ) $(LDFLAGS) $(RUBYLIBS) $(RUBYRPATH) $(MODLINKFLAGS)

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
