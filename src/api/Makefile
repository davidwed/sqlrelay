include ../../config.mk

all:
	$(MAKE) -C c++ all
	$(MAKE) -C c all
	$(MAKE) -C postgresql all
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python all
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master all
	$(MAKE) -C perl/SQLRCursor -f Makefile.master all
	$(MAKE) -C perl/DBD -f Makefile.master all
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby all
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php all
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java all
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl all
endif

clean:
	$(MAKE) -C c++ clean
	$(MAKE) -C c clean
	$(MAKE) -C postgresql clean
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python clean
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master clean
	$(MAKE) -C perl/SQLRCursor -f Makefile.master clean
	$(MAKE) -C perl/DBD -f Makefile.master clean
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby clean
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php clean
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java clean
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl clean
endif


rebuild:
	$(MAKE) -C c++ rebuild
	$(MAKE) -C c rebuild
	$(MAKE) -C postgresql rebuild
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python rebuild
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master rebuild
	$(MAKE) -C perl/SQLRCursor -f Makefile.master rebuild
	$(MAKE) -C perl/DBD -f Makefile.master rebuild
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby rebuild
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php rebuild
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java rebuild
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl rebuild
endif

install:
	$(MAKE) -C c++ install
	$(MAKE) -C c install
	$(MAKE) -C postgresql install
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python install
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master install
	$(MAKE) -C perl/SQLRCursor -f Makefile.master install
	$(MAKE) -C perl/DBD -f Makefile.master install
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby install
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php install
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java install
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl install
endif
ifneq ($(HAVE_ZOPE),)
	$(MAKE) -C zope install
endif

uninstall:
	$(MAKE) -C c++ uninstall
	$(MAKE) -C c uninstall
	$(MAKE) -C postgresql uninstall
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python uninstall
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/SQLRConnection -f Makefile.master uninstall
	$(MAKE) -C perl/SQLRCursor -f Makefile.master uninstall
	$(MAKE) -C perl/DBD -f Makefile.master uninstall
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby uninstall
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php uninstall
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java uninstall
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl uninstall
endif
ifneq ($(HAVE_ZOPE),)
	$(MAKE) -C zope uninstall
endif
