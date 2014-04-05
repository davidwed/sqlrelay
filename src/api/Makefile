include ../../config.mk

.PHONY: all clean install uninstall

all:
	$(MAKE) -C c++ all
	$(MAKE) -C c all
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MAKE) -C postgresql all
	$(MAKE) -C mysql all
ifneq ($(ODBCLIBS),)
	$(MAKE) -C odbc all
endif
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python all
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl -f Makefile all
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby all
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php all
endif
ifneq ($(HAVE_PHP_PDO),)
	$(MAKE) -C phppdo all
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
endif

clean:
	$(MAKE) -C c++ clean
	$(MAKE) -C c clean
	$(MAKE) -C postgresql clean
	$(MAKE) -C mysql clean
ifneq ($(ODBCLIBS),)
	$(MAKE) -C odbc clean
endif
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python clean
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl -f Makefile clean
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby clean
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php clean
endif
ifneq ($(HAVE_PHP_PDO),)
	$(MAKE) -C phppdo clean
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
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MAKE) -C postgresql install
	$(MAKE) -C mysql install
ifneq ($(ODBCLIBS),)
	$(MAKE) -C odbc install
endif
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python install
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl -f Makefile install
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby install
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php install
endif
ifneq ($(HAVE_PHP_PDO),)
	$(MAKE) -C phppdo install
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java install
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl install
endif
ifneq ($(HAVE_ERLANG),)
	$(MAKE) -C erlang install
endif
endif

uninstall:
	$(MAKE) -C c++ uninstall
	$(MAKE) -C c uninstall
	$(MAKE) -C postgresql uninstall
	$(MAKE) -C mysql uninstall
ifneq ($(ODBCLIBS),)
	$(MAKE) -C odbc uninstall
endif
ifneq ($(HAVE_PYTHON),)
	$(MAKE) -C python uninstall
endif
ifneq ($(HAVE_PERL),)
	$(MAKE) -C perl -f Makefile uninstall
endif
ifneq ($(HAVE_RUBY),)
	$(MAKE) -C ruby uninstall
endif
ifneq ($(HAVE_PHP),)
	$(MAKE) -C php uninstall
endif
ifneq ($(HAVE_PHP_PDO),)
	$(MAKE) -C phppdo uninstall
endif
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java uninstall
endif
ifneq ($(HAVE_TCL),)
	$(MAKE) -C tcl uninstall
endif
ifneq ($(HAVE_ERLANG),)
	$(MAKE) -C erlang uninstall
endif
