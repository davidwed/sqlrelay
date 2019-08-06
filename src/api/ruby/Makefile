top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(CP) rubyincludes1.h rubyincludes.h; \
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(RUBYCPPFLAGS) $(COMPILE) $< $(OUT)$@ 2> errors1; \
	if ( test ! -r sqlrelay.lo ); then \
		$(CP) rubyincludes2.h rubyincludes.h; \
		$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(RUBYCPPFLAGS) $(COMPILE) $< $(OUT)$@ 2> errors2; \
		if ( test ! -r sqlrelay.lo ); then \
			$(CP) rubyincludes3.h rubyincludes.h; \
			$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(RUBYCPPFLAGS) $(COMPILE) $< $(OUT)$@ 2> errors3; \
			if ( test ! -r sqlrelay.lo ); then \
				if ( test -s errors1 ); then \
					echo "first try errors : "; cat errors1; echo; \
				fi; \
				if ( test -s errors2 ); then \
					echo "second try errors : "; cat errors2; echo; \
				fi; \
				if ( test -s errors3 ); then \
					echo "third try errors : "; cat errors3; echo; \
				fi; \
				exit 1; \
			else \
				cat errors3; \
			fi; \
		else \
			cat errors2; \
		fi; \
	else \
		cat errors1; \
	fi; \
	$(RM) errors1 errors2 errors3 rubyincludes.h

.cpp.obj:
	$(CP) rubyincludes2.h rubyincludes.h
	$(CXX) $(CXXFLAGS) $(RUBYCPPFLAGS) $(COMPILE) $<

all: sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.so *.ii errors1 errors2 errors3 rubyincludes.h
	$(RMTREE) .libs

sqlrelay.$(LIBEXT): sqlrelay.cpp sqlrelay.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelay.$(OBJ) $(LDFLAGS) $(RUBYLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(RUBYSITEARCHDIR)
	$(LTINSTALL) $(CP) sqlrelay.$(LIBEXT) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.so

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(RUBYSITEARCHDIR)
	$(LTINSTALL) $(CP) sqlrelay.la $(DESTDIR)$(RUBYSITEARCHDIR)
	$(RM) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.a
	$(RM) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.la
	$(MODULERENAME) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(DESTDIR)$(RUBYSITEARCHDIR)/sqlrelay.*
