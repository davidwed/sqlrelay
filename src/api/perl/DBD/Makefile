top_builddir = ../../../..

include ../../../../config.mk

.PHONY: all clean rebuild install uninstall

PERLCCFLAGS = $(DESTDIR)$(shell eval "`$(PERL) -V:ccflags`"; export ccflags; echo $$ccflags | sed -e "s|UNKNOWN||g" )
PERLOPTIMIZE = $(DESTDIR)$(shell eval "`$(PERL) -V:optimize`"; export optimize; echo $$optimize | sed -e "s|UNKNOWN||g" )
PERLSITEARCH = $(DESTDIR)$(shell eval "`$(PERL) -V:sitearch`"; export sitearch; echo $$sitearch | sed -e "s|UNKNOWN||g" )
PERLSITELIB = $(DESTDIR)$(shell eval "`$(PERL) -V:sitelib`"; export sitelib; echo $$sitelib | sed -e "s|UNKNOWN||g" )
PERLINSTALLMAN3DIR = $(DESTDIR)$(shell eval "`$(PERL) -V:installman3dir`"; export installman3dir; echo $$installman3dir )
PERLMAN3EXT = $(shell eval "`$(PERL) -V:man3ext`"; export man3ext; echo $$man3ext | sed -e "s|UNKNOWN||g" )

all:

clean:

rebuild: clean all

install: 
	$(MKINSTALLDIRS) $(PERLSITELIB)/DBD
	$(INSTALL) -m 644 SQLRelay.pm $(PERLSITELIB)/DBD
ifeq ($(CYGWIN),)
ifneq ($(POD2MAN),)
ifneq ($(PERLINSTALLMAN3DIR),)
	$(POD2MAN) SQLRelay.pm > DBD::SQLRelay.$(PERLMAN3EXT)
	$(MKINSTALLDIRS) $(PERLINSTALLMAN3DIR)
	$(INSTALL) -m 644 DBD::SQLRelay.$(PERLMAN3EXT) $(PERLINSTALLMAN3DIR)
	$(RM) DBD::SQLRelay.$(PERLMAN3EXT)
endif
endif
endif
	$(MKINSTALLDIRS) $(PERLSITEARCH)/auto/DBD/SQLRelay
	echo $(PERLSITELIB)/DBD/SQLRelay.pm > $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist
ifeq ($(CYGWIN),)
ifneq ($(POD2MAN),)
ifneq ($(PERLINSTALLMAN3DIR),)
	echo $(PERLINSTALLMAN3DIR)/DBD::SQLRelay.$(PERLMAN3EXT) >> $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist
endif
endif
endif

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) `cat $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist`
	$(LIBTOOL) --mode=uninstall $(RM) $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist
