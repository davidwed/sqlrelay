include ../../config.mk

.PHONY: all clean install uninstall

all:
	$(MAKE) -C c++ all
	$(MAKE) -C c all
	$(MAKE) -C postgresql all
	$(MAKE) -C mysql all
ifneq ($(ODBCINSTLIBS),)
	$(MAKE) -C odbc all
endif
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
	$(MAKE) -C phppeardb all
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java all
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl all
endif
ifneq ($(HAVE_ERLANG),)
	$(MAKE) -C erlang all
endif

clean:
	$(MAKE) -C c++ clean
	$(MAKE) -C c clean
	$(MAKE) -C postgresql clean
	$(MAKE) -C mysql clean
ifneq ($(ODBCINSTLIBS),)
	$(MAKE) -C odbc clean
endif
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
	$(MAKE) -C phppeardb clean
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java clean
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl clean
endif
ifneq ($(HAVE_ERLANG),)
	$(MAKE) -C erlang clean
endif

install:
	$(MAKE) -C c++ install
	$(MAKE) -C c install
	$(MAKE) -C postgresql install
	$(MAKE) -C mysql install
ifneq ($(ODBCINSTLIBS),)
	$(MAKE) -C odbc install
endif
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
	$(MAKE) -C phppeardb install
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
ifneq ($(HAVE_ERLANG),)
	$(MAKE) -C erlang install
endif

uninstall:
	$(MAKE) -C c++ uninstall
	$(MAKE) -C c uninstall
	$(MAKE) -C postgresql uninstall
	$(MAKE) -C mysql uninstall
ifneq ($(ODBCINSTLIBS),)
	$(MAKE) -C odbc uninstall
endif
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
	$(MAKE) -C phppeardb uninstall
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
ifneq ($(HAVE_ERLANG),)
	$(MAKE) -C erlang uninstall
endif
