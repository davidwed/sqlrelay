include ../config.mk

.PHONY: all clean rebuild install uninstall

all:
	$(MAKE) -C util all
	$(MAKE) -C listener all
	$(MAKE) -C connection all
	$(MAKE) -C connections all
	$(MAKE) -C start all
	$(MAKE) -C cachemanager all
	$(MAKE) -C scaler all
ifneq ($(HAVE_GTK),)
	$(MAKE) -C config all
endif
	$(MAKE) -C api all
	$(MAKE) -C cmdline all

clean:
	$(MAKE) -C util clean
	$(MAKE) -C listener clean
	$(MAKE) -C connection clean
	$(MAKE) -C connections clean
	$(MAKE) -C start clean
	$(MAKE) -C cachemanager clean
	$(MAKE) -C scaler clean
ifneq ($(HAVE_GTK),)
	$(MAKE) -C config clean
endif
	$(MAKE) -C api clean
	$(MAKE) -C cmdline clean

rebuild:
	$(MAKE) -C util rebuild
	$(MAKE) -C listener rebuild
	$(MAKE) -C connection rebuild
	$(MAKE) -C connections rebuild
	$(MAKE) -C start rebuild
	$(MAKE) -C cachemanager rebuild
	$(MAKE) -C scaler rebuild
ifneq ($(HAVE_GTK),)
	$(MAKE) -C config rebuild
endif
	$(MAKE) -C api rebuild
	$(MAKE) -C cmdline rebuild

install:
	$(MAKE) -C util install
	$(MAKE) -C listener install
	$(MAKE) -C connection install
	$(MAKE) -C connections install
	$(MAKE) -C start install
	$(MAKE) -C cachemanager install
	$(MAKE) -C scaler install
ifneq ($(HAVE_GTK),)
	$(MAKE) -C config install
endif
	$(MAKE) -C api install
	$(MAKE) -C cmdline install

uninstall:
	$(MAKE) -C util uninstall
	$(MAKE) -C listener uninstall
	$(MAKE) -C connection uninstall
	$(MAKE) -C connections uninstall
	$(MAKE) -C start uninstall
	$(MAKE) -C cachemanager uninstall
	$(MAKE) -C scaler uninstall
ifneq ($(HAVE_GTK),)
	$(MAKE) -C config uninstall
endif
	$(MAKE) -C api uninstall
	$(MAKE) -C cmdline uninstall
