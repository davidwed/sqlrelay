include ../config.mk

.PHONY: all clean

all:
	$(MAKE) -C c all
	$(MAKE) -C c++ all
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java all
endif
	$(MAKE) -C dropin all
	$(MAKE) -C stress all

clean:
	$(MAKE) -C c clean
	$(MAKE) -C c++ clean
	$(MAKE) -C java clean
	$(MAKE) -C perl clean
	$(MAKE) -C python clean
	$(MAKE) -C ruby clean
	$(MAKE) -C tcl clean
	$(MAKE) -C dropin clean
	$(MAKE) -C stress clean
