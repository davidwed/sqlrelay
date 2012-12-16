include ../config.mk

.PHONY: all clean install uninstall

all:
	$(MAKE) -C util all
	$(MAKE) -C listener all
	$(MAKE) -C api all
	$(MAKE) -C cmdline all
	$(MAKE) -C connection all
	$(MAKE) -C triggers all
	$(MAKE) -C translations all
	$(MAKE) -C queries all
	$(MAKE) -C connections all
	$(MAKE) -C start all
	$(MAKE) -C cachemanager all
	$(MAKE) -C scaler all
	$(MAKE) -C status all

clean:
	$(MAKE) -C util clean
	$(MAKE) -C listener clean
	$(MAKE) -C api clean
	$(MAKE) -C cmdline clean
	$(MAKE) -C connection clean
	$(MAKE) -C triggers clean
	$(MAKE) -C translations clean
	$(MAKE) -C queries clean
	$(MAKE) -C connections clean
	$(MAKE) -C start clean
	$(MAKE) -C cachemanager clean
	$(MAKE) -C scaler clean
	$(MAKE) -C status clean

install:
	$(MAKE) -C util install
	$(MAKE) -C listener install
	$(MAKE) -C api install
	$(MAKE) -C cmdline install
	$(MAKE) -C connection install
	$(MAKE) -C triggers install
	$(MAKE) -C translations install
	$(MAKE) -C queries install
	$(MAKE) -C connections install
	$(MAKE) -C start install
	$(MAKE) -C cachemanager install
	$(MAKE) -C scaler install
	$(MAKE) -C status install

uninstall:
	$(MAKE) -C util uninstall
	$(MAKE) -C listener uninstall
	$(MAKE) -C api uninstall
	$(MAKE) -C cmdline uninstall
	$(MAKE) -C connection uninstall
	$(MAKE) -C triggers uninstall
	$(MAKE) -C translations uninstall
	$(MAKE) -C queries uninstall
	$(MAKE) -C connections uninstall
	$(MAKE) -C start uninstall
	$(MAKE) -C cachemanager uninstall
	$(MAKE) -C scaler uninstall
	$(MAKE) -C status uninstall
