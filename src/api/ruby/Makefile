top_builddir = ../../..

include ../../../config.mk

RUBYCFLAGS = $(shell $(RUBY) getcflags.rb | sed -e "s|-mtune=pentium4||g" | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )

ifeq ($(shell $(RUBY) getsitearchdir.rb | grep -c DESTDIR),0)
RUBYSITEARCHDIR = $(DESTDIR)$(RUBYCYGDRIVEPREFIX)$(shell $(RUBY) getsitearchdir.rb | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )
else
RUBYSITEARCHDIR = $(shell $(RUBY) getsitearchdir.rb | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )
endif

ifeq ($(shell $(RUBY) getsitelibdir.rb | grep -c DESTDIR),0)
RUBYSITELIBDIR = $(DESTDIR)$(RUBYCYGDRIVEPREFIX)$(shell $(RUBY) getsitelibdir.rb | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )
else
RUBYSITELIBDIR = $(shell $(RUBY) getsitelibdir.rb | $(MAKE) -s -f - | grep -v Entering | grep -v Leaving )
endif

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(RUBYCFLAGS) -I./ -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(RUBYLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(RUBYUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = sqlrelay.C
LOBJS = sqlrelay.lo

.SUFFIXES: .lo

.C.lo:
	cp rubyincludes1.h rubyincludes.h; \
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors1; \
	if ( test ! -r sqlrelay.lo ); then \
		cp rubyincludes2.h rubyincludes.h; \
		$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors2; \
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


all: libsqlrelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o	
	$(LIBTOOL) --mode=clean $(RM) *.so

rebuild: clean all

libsqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(RUBYDIR)/site-packages/SQLRelay $(RPATH) -module -no-undefined

sqlrelay.so:
	if ( test -r .libs/libsqlrelay.0.0.0 ); \
	then \
		cp .libs/libsqlrelay.0.0.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.0.0 ); \
	then \
		cp .libs/libsqlrelay.0.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.0 ); \
	then \
		cp .libs/libsqlrelay.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay ); \
	then \
		cp .libs/libsqlrelay sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.so.0.0.0 ); \
	then \
		cp .libs/libsqlrelay.so.0.0.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.so.0.0 ); \
	then \
		cp .libs/libsqlrelay.so.0.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.so.0 ); \
	then \
		cp .libs/libsqlrelay.so.0 sqlrelay.so; \
	fi
	if ( test -r .libs/libsqlrelay.so ); \
	then \
		cp .libs/libsqlrelay.so sqlrelay.so; \
	fi

sqlrelay.dll:
	if ( test -r .libs/cygsqlrelay-0.dll ); \
	then \
		cp .libs/cygsqlrelay-0.dll sqlrelay.dll; \
	fi

ifeq ($(CYGWIN),)
install: sqlrelay.so
	$(MKINSTALLDIRS) $(RUBYSITEARCHDIR)
	$(INSTALL) sqlrelay.so $(RUBYSITEARCHDIR)
	$(MKINSTALLDIRS) $(RUBYSITELIBDIR)/DBD/SQLRelay
	$(INSTALL) -m 644 SQLRelay.rb $(RUBYSITELIBDIR)/DBD/SQLRelay
	$(RM) sqlrelay.so

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(RUBYSITEARCHDIR)/sqlrelay.so
	$(RMTREE) $(RUBYSITELIBDIR)/DBD/SQLRelay
else
install: sqlrelay.dll
	$(MKINSTALLDIRS) $(RUBYSITEARCHDIR)
	$(INSTALL) sqlrelay.dll $(RUBYSITEARCHDIR)
	$(MKINSTALLDIRS) $(RUBYSITELIBDIR)/DBD/SQLRelay
	$(INSTALL) -m 644 SQLRelay.rb $(RUBYSITELIBDIR)/DBD/SQLRelay
	$(RM) sqlrelay.dll

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(RUBYSITEARCHDIR)/sqlrelay.dll
	$(RMTREE) $(RUBYSITELIBDIR)/DBD/SQLRelay
endif
