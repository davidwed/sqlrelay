include ../../../config.mk

.PHONY: all clean rebuild install uninstall

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

rebuild:
	$(MAKE) -C src rebuild

install:
	$(MAKE) -C include install
	$(MAKE) -C src install

uninstall:
	$(MAKE) -C include uninstall
	$(MAKE) -C src uninstall
