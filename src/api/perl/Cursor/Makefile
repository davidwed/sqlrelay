top_builddir = ../../../..

include ../../../../config.mk

PERLCCFLAGS = $(DESTDIR)$(shell eval "`$(PERL) -V:ccflags`"; export ccflags; echo $$ccflags | sed -e 's| -belf||g' -e 's| -KPIC||g' -e 's| -xdepend||g' -e 's| -x03||g' -e "s|UNKNOWN||g" )
PERLOPTIMIZE = $(DESTDIR)$(shell eval "`$(PERL) -V:optimize`"; export optimize; echo $$optimize | sed -e 's| -belf||g' -e 's| -KPIC||g' -e 's| -xdepend||g' -e 's| -x03||g' -e "s|UNKNOWN||g" )
PERLSITEARCH = $(DESTDIR)$(shell eval "`$(PERL) -V:sitearch`"; export sitearch; echo $$sitearch | sed -e "s|UNKNOWN||g" )
PERL_INC = $(DESTDIR)$(shell eval "`$(PERL) -V:archlibexp`"; export archlibexp; echo "-I$$archlibexp/CORE" | sed -e "s|UNKNOWN||g" )
PERL_LIB = $(DESTDIR)$(shell eval "`$(PERL) -V:privlibexp`"; export privlibexp; echo $$privlibexp | sed -e "s|UNKNOWN||g" )
PERLINSTALLMAN3DIR = $(DESTDIR)$(shell eval "`$(PERL) -V:installman3dir`"; export installman3dir; echo $$installman3dir | sed -e "s|UNKNOWN||g" )
PERLMAN3EXT = $(shell eval "`$(PERL) -V:man3ext`"; export man3ext; echo $$man3ext | sed -e "s|UNKNOWN||g" )

CPPFLAGS = $(BASECPPFLAGS) $(PERLOPTIMIZE) $(PERLCCFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PERL_INC)
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PERLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo .xs

.xs.C:
	$(PERL) $(PERL_LIB)/ExtUtils/xsubpp -noprototypes -C++ -typemap $(PERL_LIB)/ExtUtils/typemap -typemap perlobject.map -typemap typemap $< > $<c && mv $<c $@

.C.lo:
	cp perlincludes1.h perlincludes.h; \
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors1; \
	if ( test ! -r Cursor.lo ); then \
		cp perlincludes2.h perlincludes.h; \
		$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors2; \
		if ( test ! -r Cursor.lo ); then \
			cp perlincludes2.h perlincludes.h; \
			$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors3; \
			if ( test ! -r Cursor.lo ); then \
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
				$(STRIPCOMMENTNOTE) $@; \
			fi; \
		else \
			cat errors2; \
			$(STRIPCOMMENTNOTE) $@; \
		fi; \
	else \
		cat errors1; \
		$(STRIPCOMMENTNOTE) $@; \
	fi; \
	$(RM) errors1 errors2 errors3 perlincludes.h


all: libCursor.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.C
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo*
	$(LIBTOOL) --mode=clean $(RM) *.o	
	$(LIBTOOL) --mode=clean $(RM) *.so
	$(LIBTOOL) --mode=clean $(RM) *.$(PERLMAN3EXT)
	$(LIBTOOL) --mode=clean $(RM) errors*
	$(LIBTOOL) --mode=clean $(RM) perlincludes.h

rebuild: clean all

libCursor.la: Cursor.xs Cursor.C Cursor.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ Cursor.lo $(LIBS) -rpath $(PERLSITEARCH)/auto/SQLRelay/Cursor $(RPATH) -module

Cursor.so:
	if ( test -r .libs/libCursor.0.0.0 ); \
	then \
		cp .libs/libCursor.0.0.0 Cursor.so; \
	fi
	if ( test -r .libs/libCursor.0.0 ); \
	then \
		cp .libs/libCursor.0.0 Cursor.so; \
	fi
	if ( test -r .libs/libCursor.0 ); \
	then \
		cp .libs/libCursor.0 Cursor.so; \
	fi
	if ( test -r .libs/libCursor ); \
	then \
		cp .libs/libCursor Cursor.so; \
	fi
	if ( test -r .libs/libCursor.so.0.0.0 ); \
	then \
		cp .libs/libCursor.so.0.0.0 Cursor.so; \
	fi
	if ( test -r .libs/libCursor.so.0.0 ); \
	then \
		cp .libs/libCursor.so.0.0 Cursor.so; \
	fi
	if ( test -r .libs/libCursor.so.0 ); \
	then \
		cp .libs/libCursor.so.0 Cursor.so; \
	fi
	if ( test -r .libs/libCursor.so ); \
	then \
		cp .libs/libCursor.so Cursor.so; \
	fi

install: Cursor.so
	$(MKINSTALLDIRS) $(PERLSITEARCH)/SQLRelay
	$(INSTALL) -m 644 Cursor.pm $(PERLSITEARCH)/SQLRelay
	$(MKINSTALLDIRS) $(PERLSITEARCH)/auto/SQLRelay/Cursor
	$(INSTALL) -m 755 Cursor.so $(PERLSITEARCH)/auto/SQLRelay/Cursor
	touch $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.bs
	$(RM) Cursor.so
	pod2man Cursor.pm > SQLRelay::Cursor.$(PERLMAN3EXT)
	$(MKINSTALLDIRS) $(PERLINSTALLMAN3DIR)
	$(INSTALL) -m 644 SQLRelay::Cursor.$(PERLMAN3EXT) $(PERLINSTALLMAN3DIR)
	$(RM) SQLRelay::Cursor.$(PERLMAN3EXT)
	echo $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.so > $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.bs > $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLINSTALLMAN3DIR)/SQLRelay::Cursor.$(PERLMAN3EXT) > $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) `cat $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist`
	$(LIBTOOL) --mode=uninstall $(RM) $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist
