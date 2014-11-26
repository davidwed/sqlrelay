top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

PERLCCFLAGS_LOCAL = $(DESTDIR)$(shell echo "$(PERLCCFLAGS)" | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g" -e "s|-arch .* ||g" -e "s|-Kalloca ||g")
PERLOPTIMIZE_LOCAL = $(DESTDIR)$(shell echo "$(PERLOPTIMIZE)" | sed -e 's| -belf||g' -e 's|-KPIC||g' -e 's|-x.* | |g' -e "s|UNKNOWN||g" -e "s|-Dbool=char||g" -e "s|-mtune=.* | |g")
ifeq ($(OVERRIDEPERLSITEARCH),)
PERLSITEARCH_LOCAL = $(DESTDIR)$(shell echo "$(PERLSITEARCH)" | sed -e "s|UNKNOWN||g" )
else
PERLSITEARCH_LOCAL = $(DESTDIR)$(OVERRIDEPERLSITEARCH)
endif
ifeq ($(OVERRIDEPERLSITELIB),)
PERLSITELIB_LOCAL = $(DESTDIR)$(shell echo "$(PERLSITELIB)" | sed -e "s|UNKNOWN||g" )
else
PERLSITELIB_LOCAL = $(DESTDIR)$(OVERRIDEPERLSITELIB)
endif
PERLINC_LOCAL = $(DESTDIR)$(shell echo "$(PERLINC)" | sed -e "s|UNKNOWN||g" )
ifeq ($(OVERRIDEPERLINSTALLMAN3DIR),)
PERLINSTALLMAN3DIR_LOCAL = $(DESTDIR)$(shell echo "$(PERLINSTALLMAN3DIR)" | sed -e "s|UNKNOWN||g" )
else
PERLINSTALLMAN3DIR_LOCAL = $(DESTDIR)$(OVERRIDEPERLINSTALLMAN3DIR)
endif
ifeq ($(OVERRIDEPERLMAN3EXT),)
PERLMAN3EXT_LOCAL = $(shell eval "$(PERLMAN3EXT)" | sed -e "s|UNKNOWN||g" )
else
PERLMAN3EXT_LOCAL = $(OVERRIDEPERLMAN3EXT)
endif

CPPFLAGS = $(BASECPPFLAGS) $(PERLOPTIMIZE_LOCAL) $(PERLCCFLAGS_LOCAL) $(PERL_500) -I./ -I$(top_builddir) -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(PERLINC_LOCAL)
LIBS = $(PERLLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PERLUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo .xs

.xs.cpp:
	$(PERL) $(XSUBPP) -C++ -typemap $(PERLLIB)/ExtUtils/typemap -typemap perlobject.map -typemap typemap $< > $<c && mv $<c $@

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: Connection.la Cursor.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.cpp *.la *.lo* *.o *.so *.$(PERLMAN3EXT_LOCAL) errors*
	$(RMTREE) .libs

Connection.la: Connection.xs Connection.cpp Connection.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ Connection.lo $(LDFLAGS) $(LIBS) -rpath $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection $(RPATH) -avoid-version -module -no-undefined

Cursor.la: Cursor.xs Cursor.cpp Cursor.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ Cursor.lo $(LDFLAGS) $(LIBS) -rpath $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor $(RPATH) -avoid-version -module -no-undefined


install:
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/SQLRelay
	$(INSTALL) -m 644 Connection.pm $(PERLSITEARCH_LOCAL)/SQLRelay
	$(INSTALL) -m 644 Cursor.pm $(PERLSITEARCH_LOCAL)/SQLRelay
	$(MKINSTALLDIRS) $(PERLSITELIB_LOCAL)/DBD
	$(INSTALL) -m 644 SQLRelay.pm $(PERLSITELIB_LOCAL)/DBD
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay
	$(LTINSTALL) -m 755 Connection.la $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection
	$(LTINSTALL) -m 755 Cursor.la $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.a
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.la
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.a
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.la
	$(MODULERENAME) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.so so $(MODULESUFFIX)
	$(MODULERENAME) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.so so $(MODULESUFFIX)
	touch $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.bs
	touch $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.bs
	echo $(PERLSITEARCH_LOCAL)/SQLRelay/Connection.pm > $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH_LOCAL)/SQLRelay/Cursor.pm > $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.$(MODULESUFFIX) >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.$(MODULESUFFIX) >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.bs >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.bs >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLSITELIB_LOCAL)/DBD/SQLRelay.pm >> $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay/.packlist
ifneq ($(POD2MAN),)
ifneq ($(PERLINSTALLMAN3DIR_LOCAL),)
	$(POD2MAN) Connection.pm > SQLRelay::Connection.$(PERLMAN3EXT_LOCAL)
	$(POD2MAN) Cursor.pm > SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL)
	$(POD2MAN) SQLRelay.pm > DBD::SQLRelay.$(PERLMAN3EXT_LOCAL)
	$(MKINSTALLDIRS) $(PERLINSTALLMAN3DIR_LOCAL)
	$(INSTALL) -m 644 SQLRelay::Connection.$(PERLMAN3EXT_LOCAL) $(PERLINSTALLMAN3DIR_LOCAL)
	$(INSTALL) -m 644 SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL) $(PERLINSTALLMAN3DIR)
	$(INSTALL) -m 644 DBD::SQLRelay.$(PERLMAN3EXT_LOCAL) $(PERLINSTALLMAN3DIR_LOCAL)
	$(RM) SQLRelay::Connection.$(PERLMAN3EXT_LOCAL)
	$(RM) SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL)
	$(RM) DBD::SQLRelay.$(PERLMAN3EXT_LOCAL)
	echo $(PERLINSTALLMAN3DIR_LOCAL)/SQLRelay::Connection.$(PERLMAN3EXT_LOCAL) >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	echo $(PERLINSTALLMAN3DIR_LOCAL)/SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL) >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLINSTALLMAN3DIR_LOCAL)/DBD::SQLRelay.$(PERLMAN3EXT_LOCAL) >> $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay/.packlist
endif
endif

uninstall:
	$(RM) `cat $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist`
	$(RM) `cat $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist`
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	$(RM) `cat $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay/.packlist`
	$(RM) $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay/.packlist
	$(RMTREE) $(PERLSITEARCH_LOCAL)/auto/SQLRelay
	$(RMTREE) $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay
	$(RMTREE) $(PERLSITEARCH_LOCAL)/SQLRelay
