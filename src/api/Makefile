include ../../config.mk

all:
	$(MAKE) -C c++ all
	$(MAKE) -C c all
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python all
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master all
	$(MAKE) -C perl/SQLRCursor -f Makefile.master all
	$(MAKE) -C perl/DBD -f Makefile.master all
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby -f Makefile.master all
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php all
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java all
endif

clean:
	$(MAKE) -C c++ clean
	$(MAKE) -C c clean
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python clean
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master clean
	$(MAKE) -C perl/SQLRCursor -f Makefile.master clean
	$(MAKE) -C perl/DBD -f Makefile.master clean
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby -f Makefile.master clean
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php clean
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java clean
endif


rebuild:
	$(MAKE) -C c++ rebuild
	$(MAKE) -C c rebuild
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python rebuild
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master rebuild
	$(MAKE) -C perl/SQLRCursor -f Makefile.master rebuild
	$(MAKE) -C perl/DBD -f Makefile.master rebuild
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby -f Makefile.master rebuild
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php rebuild
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java rebuild
endif

install:
	$(MAKE) -C c++ install
	$(MAKE) -C c install
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python install
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master install
	$(MAKE) -C perl/SQLRCursor -f Makefile.master install
	$(MAKE) -C perl/DBD -f Makefile.master install
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby -f Makefile.master install
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php install
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java install
endif

uninstall:
	$(MAKE) -C c++ uninstall
	$(MAKE) -C c uninstall
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python uninstall
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master uninstall
	$(MAKE) -C perl/SQLRCursor -f Makefile.master uninstall
	$(MAKE) -C perl/DBD -f Makefile.master uninstall
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby -f Makefile.master uninstall
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php uninstall
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java uninstall
endif
