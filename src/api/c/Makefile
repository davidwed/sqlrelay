include ../../../config.mk

.PHONY: all clean rebuild install uninstall

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

rebuild:
	$(MAKE) -C src rebuild

install:
	$(MAKE) -C src install
	$(MAKE) -C include install

uninstall:
	$(MAKE) -C src uninstall
	$(MAKE) -C include uninstall
