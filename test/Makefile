include ../config.mk

.PHONY: all clean rebuild

all:
	$(MAKE) -C c all
	$(MAKE) -C c++ all
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java all
endif
	$(MAKE) -C dropin all

clean:
	$(MAKE) -C c clean
	$(MAKE) -C c++ clean
	$(MAKE) -C java clean
	$(MAKE) -C perl clean
	$(MAKE) -C python clean
	$(MAKE) -C ruby clean
	$(MAKE) -C dropin clean

rebuild: clean all
