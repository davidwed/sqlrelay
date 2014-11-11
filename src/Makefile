include ../config.mk

.PHONY: all clean install uninstall

all:
	$(MAKE) -C util all
	$(MAKE) -C api all
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MAKE) -C server all
endif
	$(MAKE) -C parsers all
	$(MAKE) -C queries all
	$(MAKE) -C loggers all
	$(MAKE) -C protocols all
	$(MAKE) -C pwdencs all
	$(MAKE) -C auths all
	$(MAKE) -C connections all
ifeq ($(SQLRELAY_ENABLE_SHARED),no)
	$(MAKE) -C server all
endif
	$(MAKE) -C cmdline all

clean:
	$(MAKE) -C util clean
	$(MAKE) -C api clean
	$(MAKE) -C parsers clean
	$(MAKE) -C queries clean
	$(MAKE) -C loggers clean
	$(MAKE) -C protocols clean
	$(MAKE) -C pwdencs clean
	$(MAKE) -C auths clean
	$(MAKE) -C connections clean
	$(MAKE) -C server clean
	$(MAKE) -C cmdline clean

install:
	$(MAKE) -C util install
	$(MAKE) -C api install
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MAKE) -C server install
endif
	$(MAKE) -C parsers install
	$(MAKE) -C queries install
	$(MAKE) -C loggers install
	$(MAKE) -C protocols install
	$(MAKE) -C pwdencs install
	$(MAKE) -C auths install
	$(MAKE) -C connections install
ifeq ($(SQLRELAY_ENABLE_SHARED),no)
	$(MAKE) -C server install
endif
	$(MAKE) -C cmdline install

uninstall:
	$(MAKE) -C util uninstall
	$(MAKE) -C api uninstall
	$(MAKE) -C parsers uninstall
	$(MAKE) -C queries uninstall
	$(MAKE) -C loggers uninstall
	$(MAKE) -C protocols uninstall
	$(MAKE) -C pwdencs uninstall
	$(MAKE) -C auths uninstall
	$(MAKE) -C connections uninstall
	$(MAKE) -C server uninstall
	$(MAKE) -C cmdline uninstall
