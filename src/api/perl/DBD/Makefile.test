top_builddir = ../../../..

include ../../../../config.mk

PERLCCFLAGS = $(DESTDIR)$(shell eval "export `$(PERL) -V:ccflags`"; echo $$ccflags )
PERLOPTIMIZE = $(DESTDIR)$(shell eval "export `$(PERL) -V:optimize`"; echo $$optimize )
PERLSITEARCH = $(DESTDIR)$(shell eval "export `$(PERL) -V:sitearch`"; echo $$sitearch )
PERLSITELIB = $(DESTDIR)$(shell eval "export `$(PERL) -V:sitelib`"; echo $$sitelib )
PERLINSTALLMAN3DIR = $(DESTDIR)$(shell eval "export `$(PERL) -V:installman3dir`"; echo $$installman3dir )
PERLMAN3EXT = $(shell eval "export `$(PERL) -V:man3ext`"; echo $$man3ext )

all:

clean:

rebuild: clean all

install: 
	$(MKINSTALLDIRS) $(PERLSITELIB)/DBD
	$(LIBTOOL) --mode=install $(INSTALL) -m 644 SQLRelay.pm $(PERLSITELIB)/DBD
	pod2man SQLRelay.pm > DBD::SQLRelay.$(PERLMAN3EXT)
	$(MKINSTALLDIRS) $(PERLINSTALLMAN3DIR)
	$(LIBTOOL) --mode=install $(INSTALL) -m 644 DBD::SQLRelay.$(PERLMAN3EXT) $(PERLINSTALLMAN3DIR)
	$(RM) DBD::SQLRelay.$(PERLMAN3EXT)
	$(MKINSTALLDIRS) $(PERLSITEARCH)/auto/DBD/SQLRelay
	echo $(PERLSITELIB)/DBD/SQLRelay.pm > $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist
	echo $(PERLINSTALLMAN3DIR)/DBD::SQLRelay.$(PERLMAN3EXT) >> $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) `cat $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist`
	$(LIBTOOL) --mode=uninstall $(RM) $(PERLSITEARCH)/auto/DBD/SQLRelay/.packlist
