include ../config.mk

all:
	$(MAKE) -C c all
	$(MAKE) -C c++ all
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java all
endif

clean:
	$(MAKE) -C c clean
	$(MAKE) -C c++ clean
	$(MAKE) -C java clean
	$(MAKE) -C perl clean
	$(MAKE) -C python clean
	$(MAKE) -C ruby clean

rebuild: clean all
