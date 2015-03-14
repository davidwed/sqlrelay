include ../config.mk

all:
	cd util $(AND) $(MAKE) all
	cd api $(AND) $(MAKE) all
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	cd server $(AND) $(MAKE) all
endif
	cd parsers $(AND) $(MAKE) all
	cd queries $(AND) $(MAKE) all
	cd loggers $(AND) $(MAKE) all
	cd protocols $(AND) $(MAKE) all
	cd pwdencs $(AND) $(MAKE) all
	cd auths $(AND) $(MAKE) all
	cd connections $(AND) $(MAKE) all
ifeq ($(SQLRELAY_ENABLE_SHARED),no)
	cd server $(AND) $(MAKE) all
endif
	cd cmdline $(AND) $(MAKE) all

clean:
	cd util $(AND) $(MAKE) clean
	cd api $(AND) $(MAKE) clean
	cd parsers $(AND) $(MAKE) clean
	cd queries $(AND) $(MAKE) clean
	cd loggers $(AND) $(MAKE) clean
	cd protocols $(AND) $(MAKE) clean
	cd pwdencs $(AND) $(MAKE) clean
	cd auths $(AND) $(MAKE) clean
	cd connections $(AND) $(MAKE) clean
	cd server $(AND) $(MAKE) clean
	cd cmdline $(AND) $(MAKE) clean

install:
	cd util $(AND) $(MAKE) install
	cd api $(AND) $(MAKE) install
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	cd server $(AND) $(MAKE) install
endif
	cd parsers $(AND) $(MAKE) install
	cd queries $(AND) $(MAKE) install
	cd loggers $(AND) $(MAKE) install
	cd protocols $(AND) $(MAKE) install
	cd pwdencs $(AND) $(MAKE) install
	cd auths $(AND) $(MAKE) install
	cd connections $(AND) $(MAKE) install
ifeq ($(SQLRELAY_ENABLE_SHARED),no)
	cd server $(AND) $(MAKE) install
endif
	cd cmdline $(AND) $(MAKE) install

uninstall:
	cd util $(AND) $(MAKE) uninstall
	cd api $(AND) $(MAKE) uninstall
	cd parsers $(AND) $(MAKE) uninstall
	cd queries $(AND) $(MAKE) uninstall
	cd loggers $(AND) $(MAKE) uninstall
	cd protocols $(AND) $(MAKE) uninstall
	cd pwdencs $(AND) $(MAKE) uninstall
	cd auths $(AND) $(MAKE) uninstall
	cd connections $(AND) $(MAKE) uninstall
	cd server $(AND) $(MAKE) uninstall
	cd cmdline $(AND) $(MAKE) uninstall
