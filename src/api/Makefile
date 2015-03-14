include ../../config.mk

all:
	cd c++ $(AND) $(MAKE) all
	cd c $(AND) $(MAKE) all
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	cd postgresql $(AND) $(MAKE) all
	cd mysql $(AND) $(MAKE) all
ifneq ($(ODBCLIBS),)
	cd odbc $(AND) $(MAKE) all
endif
ifneq ($(HAVE_PYTHON),)
	cd python $(AND) $(MAKE) all
endif
ifneq ($(HAVE_PERL),)
	cd perl $(AND) $(MAKE) all
endif
ifneq ($(HAVE_RUBY),)
	cd ruby $(AND) $(MAKE) all
endif
ifneq ($(HAVE_PHP),)
	cd php $(AND) $(MAKE) all
endif
ifneq ($(HAVE_PHP_PDO),)
	cd phppdo $(AND) $(MAKE) all
endif
ifneq ($(HAVE_JAVA),)
	cd java $(AND) $(MAKE) all
endif
ifneq ($(HAVE_TCL),)
	cd tcl $(AND) $(MAKE) all
endif
ifneq ($(HAVE_ERLANG),)
	cd erlang $(AND) $(MAKE) all
endif
endif

clean:
	cd c++ $(AND) $(MAKE) clean
	cd c $(AND) $(MAKE) clean
	cd postgresql $(AND) $(MAKE) clean
	cd mysql $(AND) $(MAKE) clean
ifneq ($(ODBCLIBS),)
	cd odbc $(AND) $(MAKE) clean
endif
ifneq ($(HAVE_PYTHON),)
	cd python $(AND) $(MAKE) clean
endif
ifneq ($(HAVE_PERL),)
	cd perl $(AND) $(MAKE) clean
endif
ifneq ($(HAVE_RUBY),)
	cd ruby $(AND) $(MAKE) clean
endif
ifneq ($(HAVE_PHP),)
	cd php $(AND) $(MAKE) clean
endif
ifneq ($(HAVE_PHP_PDO),)
	cd phppdo $(AND) $(MAKE) clean
endif
ifneq ($(HAVE_JAVA),)
	cd java $(AND) $(MAKE) clean
endif
ifneq ($(HAVE_TCL),)
	cd tcl $(AND) $(MAKE) clean
endif
ifneq ($(HAVE_ERLANG),)
	cd erlang $(AND) $(MAKE) clean
endif

install:
	cd c++ $(AND) $(MAKE) install
	cd c $(AND) $(MAKE) install
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	cd postgresql $(AND) $(MAKE) install
	cd mysql $(AND) $(MAKE) install
ifneq ($(ODBCLIBS),)
	cd odbc $(AND) $(MAKE) install
endif
ifneq ($(HAVE_PYTHON),)
	cd python $(AND) $(MAKE) install
endif
ifneq ($(HAVE_PERL),)
	cd perl $(AND) $(MAKE) install
endif
ifneq ($(HAVE_RUBY),)
	cd ruby $(AND) $(MAKE) install
endif
ifneq ($(HAVE_PHP),)
	cd php $(AND) $(MAKE) install
endif
ifneq ($(HAVE_PHP_PDO),)
	cd phppdo $(AND) $(MAKE) install
endif
ifneq ($(HAVE_JAVA),)
	cd java $(AND) $(MAKE) install
endif
ifneq ($(HAVE_TCL),)
	cd tcl $(AND) $(MAKE) install
endif
ifneq ($(HAVE_ERLANG),)
	cd erlang $(AND) $(MAKE) install
endif
endif

uninstall:
	cd c++ $(AND) $(MAKE) uninstall
	cd c $(AND) $(MAKE) uninstall
	cd postgresql $(AND) $(MAKE) uninstall
	cd mysql $(AND) $(MAKE) uninstall
ifneq ($(ODBCLIBS),)
	cd odbc $(AND) $(MAKE) uninstall
endif
ifneq ($(HAVE_PYTHON),)
	cd python $(AND) $(MAKE) uninstall
endif
ifneq ($(HAVE_PERL),)
	cd perl $(AND) $(MAKE) uninstall
endif
ifneq ($(HAVE_RUBY),)
	cd ruby $(AND) $(MAKE) uninstall
endif
ifneq ($(HAVE_PHP),)
	cd php $(AND) $(MAKE) uninstall
endif
ifneq ($(HAVE_PHP_PDO),)
	cd phppdo $(AND) $(MAKE) uninstall
endif
ifneq ($(HAVE_JAVA),)
	cd java $(AND) $(MAKE) uninstall
endif
ifneq ($(HAVE_TCL),)
	cd tcl $(AND) $(MAKE) uninstall
endif
ifneq ($(HAVE_ERLANG),)
	cd erlang $(AND) $(MAKE) uninstall
endif
