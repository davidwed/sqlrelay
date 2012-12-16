include ../config.mk

.PHONY: all clean install uninstall

all:
	$(MAKE) -C util all
	$(MAKE) -C server all
	$(MAKE) -C api all
	$(MAKE) -C cmdline all
	$(MAKE) -C connection all
	$(MAKE) -C triggers all
	$(MAKE) -C translations all
	$(MAKE) -C queries all
	$(MAKE) -C loggers all
	$(MAKE) -C pwdencs all
	$(MAKE) -C connections all
	$(MAKE) -C status all

clean:
	$(MAKE) -C util clean
	$(MAKE) -C server clean
	$(MAKE) -C api clean
	$(MAKE) -C cmdline clean
	$(MAKE) -C connection clean
	$(MAKE) -C triggers clean
	$(MAKE) -C translations clean
	$(MAKE) -C queries clean
	$(MAKE) -C loggers clean
	$(MAKE) -C pwdencs clean
	$(MAKE) -C connections clean
	$(MAKE) -C status clean

install:
	$(MAKE) -C util install
	$(MAKE) -C server install
	$(MAKE) -C api install
	$(MAKE) -C cmdline install
	$(MAKE) -C connection install
	$(MAKE) -C triggers install
	$(MAKE) -C translations install
	$(MAKE) -C queries install
	$(MAKE) -C loggers install
	$(MAKE) -C pwdencs install
	$(MAKE) -C connections install
	$(MAKE) -C status install

uninstall:
	$(MAKE) -C util uninstall
	$(MAKE) -C server uninstall
	$(MAKE) -C api uninstall
	$(MAKE) -C cmdline uninstall
	$(MAKE) -C connection uninstall
	$(MAKE) -C triggers uninstall
	$(MAKE) -C translations uninstall
	$(MAKE) -C queries uninstall
	$(MAKE) -C loggers uninstall
	$(MAKE) -C pwdencs uninstall
	$(MAKE) -C connections uninstall
	$(MAKE) -C status uninstall
