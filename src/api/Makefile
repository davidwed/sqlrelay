include ../../config.mk

all:
	$(MAKE) -C c++ all
	$(MAKE) -C c all
	$(MAKE) -C postgresql all
	$(MAKE) -C mysql all
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python all
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/Connection -f Makefile all
	$(MAKE) -C perl/Cursor -f Makefile all
	$(MAKE) -C perl/DBD -f Makefile all
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
	$(MAKE) -C mysql clean
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python clean
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/Connection -f Makefile clean
	$(MAKE) -C perl/Cursor -f Makefile clean
	$(MAKE) -C perl/DBD -f Makefile clean
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
	$(MAKE) -C mysql rebuild
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python rebuild
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/Connection -f Makefile rebuild
	$(MAKE) -C perl/Cursor -f Makefile rebuild
	$(MAKE) -C perl/DBD -f Makefile rebuild
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
	$(MAKE) -C mysql install
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python install
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/Connection -f Makefile install
	$(MAKE) -C perl/Cursor -f Makefile install
	$(MAKE) -C perl/DBD -f Makefile install
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
	$(MAKE) -C mysql uninstall
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python uninstall
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl/Connection -f Makefile uninstall
	$(MAKE) -C perl/Cursor -f Makefile uninstall
	$(MAKE) -C perl/DBD -f Makefile uninstall
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
