include ../../../config.mk

.PHONY: all clean install uninstall

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

install:
	$(MAKE) -C include install
	$(MAKE) -C src install

uninstall:
	$(MAKE) -C include uninstall
	$(MAKE) -C src uninstall
