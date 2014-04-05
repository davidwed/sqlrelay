top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

PERLCCFLAGS = $(DESTDIR)$(shell eval "`$(PERL) -V:ccflags`"; export ccflags; echo "$$ccflags " | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g" -e "s|-arch .* ||g" -e "s|-Kalloca ||g")
PERLOPTIMIZE = $(DESTDIR)$(shell eval "`$(PERL) -V:optimize`"; export optimize; echo "$$optimize " | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g")
ifeq ($(OVERRIDEPERLSITEARCH),)
PERLSITEARCH = $(DESTDIR)$(shell eval "`$(PERL) -V:sitearch`"; export sitearch; echo $$sitearch | sed -e "s|UNKNOWN||g" )
else
PERLSITEARCH = $(DESTDIR)$(OVERRIDEPERLSITEARCH)
endif
ifeq ($(OVERRIDEPERLSITELIB),)
PERLSITELIB = $(DESTDIR)$(shell eval "`$(PERL) -V:sitelib`"; export sitelib; echo $$sitelib | sed -e "s|UNKNOWN||g" )
else
PERLSITELIB = $(DESTDIR)$(OVERRIDEPERLSITELIB)
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
	$(PERL) $(XSUBPP) -noprototypes -C++ -typemap $(PERL_LIB)/ExtUtils/typemap -typemap perlobject.map -typemap typemap $< > $<c && mv $<c $@

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: Connection.la Cursor.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.cpp *.la *.lo* *.o *.so *.$(PERLMAN3EXT) errors*
	$(RMTREE) .libs

Connection.la: Connection.xs Connection.cpp Connection.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ Connection.lo $(LDFLAGS) $(LIBS) -rpath $(PERLSITEARCH)/auto/SQLRelay/Connection $(RPATH) -avoid-version -module -no-undefined

Cursor.la: Cursor.xs Cursor.cpp Cursor.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ Cursor.lo $(LDFLAGS) $(LIBS) -rpath $(PERLSITEARCH)/auto/SQLRelay/Cursor $(RPATH) -avoid-version -module -no-undefined


install:
	$(MKINSTALLDIRS) $(PERLSITEARCH)/SQLRelay
	$(INSTALL) -m 644 Connection.pm $(PERLSITEARCH)/SQLRelay
	$(INSTALL) -m 644 Cursor.pm $(PERLSITEARCH)/SQLRelay
	$(MKINSTALLDIRS) $(PERLSITEARCH)/auto/SQLRelay/Connection
	$(MKINSTALLDIRS) $(PERLSITEARCH)/auto/SQLRelay/Cursor
	$(LTINSTALL) -m 755 Connection.la $(PERLSITEARCH)/auto/SQLRelay/Connection
	$(LTINSTALL) -m 755 Cursor.la $(PERLSITEARCH)/auto/SQLRelay/Cursor
	$(RM) $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.a
	$(RM) $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.la
	$(RM) $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.a
	$(RM) $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.la
	$(MODULERENAME) $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.so so $(MODULESUFFIX)
	$(MODULERENAME) $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.so so $(MODULESUFFIX)
	touch $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.bs
	touch $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.bs
ifneq ($(POD2MAN),)
ifneq ($(PERLINSTALLMAN3DIR),)
	$(POD2MAN) Connection.pm > SQLRelay::Connection.$(PERLMAN3EXT)
	$(POD2MAN) Cursor.pm > SQLRelay::Cursor.$(PERLMAN3EXT)
	$(MKINSTALLDIRS) $(PERLINSTALLMAN3DIR)
	$(INSTALL) -m 644 SQLRelay::Connection.$(PERLMAN3EXT) $(PERLINSTALLMAN3DIR)
	$(INSTALL) -m 644 SQLRelay::Cursor.$(PERLMAN3EXT) $(PERLINSTALLMAN3DIR)
	$(RM) SQLRelay::Connection.$(PERLMAN3EXT)
	$(RM) SQLRelay::Cursor.$(PERLMAN3EXT)
	echo $(PERLINSTALLMAN3DIR)/SQLRelay::Connection.$(PERLMAN3EXT) > $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
	echo $(PERLINSTALLMAN3DIR)/SQLRelay::Cursor.$(PERLMAN3EXT) > $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist
endif
endif
	echo $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.$(MODULESUFFIX) > $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.$(MODULESUFFIX) > $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLSITEARCH)/auto/SQLRelay/Connection/Connection.bs >> $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH)/auto/SQLRelay/Cursor/Cursor.bs >> $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist

uninstall:
	$(RM) `cat $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist`
	$(RM) `cat $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist`
	$(RM) $(PERLSITEARCH)/auto/SQLRelay/Connection/.packlist
	$(RM) $(PERLSITEARCH)/auto/SQLRelay/Cursor/.packlist
	$(RM) `cat $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist`
	$(RM) $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist
