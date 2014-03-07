include ../config.mk

.PHONY: all clean install uninstall

all:
	$(MAKE) -C util all
	$(MAKE) -C api all
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MAKE) -C server all
endif
ifeq ($(TRIGGERS),yes)
	$(MAKE) -C triggers all
endif
ifeq ($(TRANSLATIONS),yes)
	$(MAKE) -C translations all
endif
	$(MAKE) -C queries all
	$(MAKE) -C loggers all
	$(MAKE) -C pwdencs all
	$(MAKE) -C connections all
ifeq ($(SQLRELAY_ENABLE_SHARED),no)
	$(MAKE) -C server all
endif
	$(MAKE) -C status all
	$(MAKE) -C cmdline all

clean:
	$(MAKE) -C util clean
	$(MAKE) -C api clean
	$(MAKE) -C triggers clean
	$(MAKE) -C translations clean
	$(MAKE) -C queries clean
	$(MAKE) -C loggers clean
	$(MAKE) -C pwdencs clean
	$(MAKE) -C connections clean
	$(MAKE) -C status clean
	$(MAKE) -C server clean
	$(MAKE) -C cmdline clean

install:
	$(MAKE) -C util install
	$(MAKE) -C api install
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MAKE) -C server install
endif
ifeq ($(TRIGGERS),yes)
	$(MAKE) -C triggers install
endif
ifeq ($(TRANSLATIONS),yes)
	$(MAKE) -C translations install
endif
	$(MAKE) -C queries install
	$(MAKE) -C loggers install
	$(MAKE) -C pwdencs install
	$(MAKE) -C connections install
ifeq ($(SQLRELAY_ENABLE_SHARED),no)
	$(MAKE) -C server install
endif
	$(MAKE) -C status install
	$(MAKE) -C cmdline install

uninstall:
	$(MAKE) -C util uninstall
	$(MAKE) -C api uninstall
	$(MAKE) -C triggers uninstall
	$(MAKE) -C translations uninstall
	$(MAKE) -C queries uninstall
	$(MAKE) -C loggers uninstall
	$(MAKE) -C pwdencs uninstall
	$(MAKE) -C connections uninstall
	$(MAKE) -C status uninstall
	$(MAKE) -C server uninstall
	$(MAKE) -C cmdline uninstall
