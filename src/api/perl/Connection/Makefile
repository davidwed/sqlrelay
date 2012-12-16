top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all clean install uninstall

PERLCCFLAGS = $(DESTDIR)$(shell eval "`$(PERL) -V:ccflags`"; export ccflags; echo "$$ccflags " | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g" -e "s|-arch .* ||g" -e "s|-Kalloca ||g")
PERLOPTIMIZE = $(DESTDIR)$(shell eval "`$(PERL) -V:optimize`"; export optimize; echo "$$optimize " | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g")
ifeq ($(OVERRIDEPERLSITEARCH),)
PERLSITEARCH = $(DESTDIR)$(shell eval "`$(PERL) -V:sitearch`"; export sitearch; echo $$sitearch | sed -e "s|UNKNOWN||g" )
else
PERLSITEARCH = $(DESTDIR)$(OVERRIDEPERLSITEARCH)
endif
PERL_INC = $(DESTDIR)$(shell eval "`$(PERL) -V:archlibexp`"; export archlibexp; echo "-I$$archlibexp/CORE" | sed -e "s|UNKNOWN||g" )
PERL_LIB = $(DESTDIR)$(shell eval "`$(PERL) -V:privlibexp`"; export privlibexp; echo $$privlibexp | sed -e "s|UNKNOWN||g" )
ifeq ($(OVERRIDEPERLINSTALLMAN3DIR),)
PERLINSTALLMAN3DIR = $(DESTDIR)$(shell eval "`$(PERL) -V:installman3dir`"; export installman3dir; echo $$installman3dir | sed -e "s|UNKNOWN||g" )
else
PERLINSTALLMAN3DIR = $(DESTDIR)$(OVERRIDEPERLINSTALLMAN3DIR)
endif
ifeq ($(OVERRIDEPERLMAN3EXT),)
PERLMAN3EXT = $(shell eval "`$(PERL) -V:man3ext`"; export man3ext; echo $$man3ext | sed -e "s|UNKNOWN||g" )
else
PERLMAN3EXT = $(OVERRIDEPERLMAN3EXT)
endif

CPPFLAGS = $(BASECPPFLAGS) $(PERLOPTIMIZE) $(PERLCCFLAGS) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PERL_INC)
LIBS = $(PERLLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PERLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo .xs

.xs.cpp:
	$(PERL) $(PERL_LIB)/ExtUtils/xsubpp -noprototypes -C++ -typemap $(PERL_LIB)/ExtUtils/typemap -typemap perlobject.map -typemap typemap $< > $<c && mv $<c $@

.cpp.lo:
	cp perlincludes1.h perlincludes.h; \
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors1; \
	if ( test ! -r Connection.lo ); then \
		cp perlincludes2.h perlincludes.h; \
		$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors2; \
		if ( test ! -r Connection.lo ); then \
			cp perlincludes3.h perlincludes.h; \
			$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors3; \
			if ( test ! -r Connection.lo ); then \
				cp perlincludes4.h perlincludes.h; \
				$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 2> errors4; \
				if ( test ! -r Connection.lo ); then \
					if ( test -s errors1 ); then \
						echo "first try errors : "; cat errors1; echo; \
					fi; \
					if ( test -s errors2 ); then \
						echo "second try errors : "; cat errors2; echo; \
					fi; \
					if ( test -s errors3 ); then \
						echo "third try errors : "; cat errors3; echo; \
					fi; \
					if ( test -s errors4 ); then \
						echo "fourth try errors : "; cat errors4; echo; \
					fi; \
					exit 1; \
				else \
					cat errors4; \
				fi; \
			else \
				cat errors3; \
			fi; \
		else \
			cat errors2; \
		fi; \
	else \
		cat errors1; \
	fi; \
	$(RM) errors1 errors2 errors3 errors4 perlincludes.h


all: libConnection.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.cpp *.la *.lo* *.o *.so *.$(PERLMAN3EXT) errors* perlincludes.h
	$(RMTREE) .libs

libConnection.la: Connection.xs Connection.cpp Connection.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ Connection.lo $(LDFLAGS) $(LIBS) -rpath $(PERLSITEARCH)/auto/SQLRelay/Connection $(RPATH) -module -no-undefined

Connection.so:
	if ( test -r .libs/libConnection.0.0.0 ); \
	then \
		cp .libs/libConnection.0.0.0 Connection.so; \
	fi
	if ( test -r .libs/libConnection.0.0 ); \
	then \
		cp .libs/libConnection.0.0 Connection.so; \
	fi
	if ( test -r .libs/libConnection.0 ); \
	then \
		cp .libs/libConnection.0 Connection.so; \
	fi
	if ( test -r .libs/libConnection ); \
	then \
		cp .libs/libConnection Connection.so; \
	fi
	if ( test -r .libs/libConnection.so.0.0.0 ); \
	then \
		cp .libs/libConnection.so.0.0.0 Connection.so; \
	fi
	if ( test -r .libs/libConnection.so.0.0 ); \
	then \
		cp .libs/libConnection.so.0.0 Connection.so; \
	fi
	if ( test -r .libs/libConnection.so.0 ); \
	then \
		cp .libs/libConnection.so.0 Connection.so; \
	fi
	if ( test -r .libs/libConnection.so ); \
	then \
		cp .libs/libConnection.so Connection.so; \
	fi

Connection.dll:
	if ( test -r .libs/cygConnection-0.dll ); \
	then \
		cp .libs/cygConnection-0.dll Connection.dll; \
	fi

ifeq ($(CYGWIN),)
install: Connection.so
	$(MKINSTALLDIRS) $(PERLSITEARCH)/SQLRelay
	$(INSTALL) -m 644 Connection.pm $(PERLSITEARCH)/SQLRelay
	$(MKINSTALLDIRS) $(PERLSITEARCH)/auto/SQLRelay/Connection
	$(LTINSTALL) -m 755 Connection.so $(PERLSITEARCH)/auto/SQLRelay/Connection
	touch $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.bs
	$(RM) Connection.so
ifneq ($(POD2MAN),)
ifneq ($(PERLINSTALLMAN3DIR),)
	$(POD2MAN) Connection.pm > SQLRelay::Connection.$(PERLMAN3EXT)
	$(MKINSTALLDIRS) $(PERLINSTALLMAN3DIR)
	$(INSTALL) -m 644 SQLRelay::Connection.$(PERLMAN3EXT) $(PERLINSTALLMAN3DIR)
	$(RM) SQLRelay::Connection.$(PERLMAN3EXT)
	echo $(PERLINSTALLMAN3DIR)/SQLRelay::Connection.$(PERLMAN3EXT) > $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
endif
endif
	echo $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.so > $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.bs > $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
else
install: Connection.dll
	$(MKINSTALLDIRS) $(PERLSITEARCH)/SQLRelay
	$(INSTALL) -m 644 Connection.pm $(PERLSITEARCH)/SQLRelay
	$(MKINSTALLDIRS) $(PERLSITEARCH)/auto/SQLRelay/Connection
	$(LTINSTALL) -m 755 Connection.dll $(PERLSITEARCH)/auto/SQLRelay/Connection
	touch $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.bs
	$(RM) Connection.dll
	echo $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.dll > $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.bs > $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
endif

uninstall:
	$(RM) `cat $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist`
	$(RM) $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
