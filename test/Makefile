include ../config.mk

all:
	$(MAKE) -C c++ all
	$(MAKE) -C c all
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java all
endif

clean:
	$(MAKE) -C c++ clean
	$(MAKE) -C c clean
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java clean
endif

rebuild:
	$(MAKE) -C c++ rebuild
	$(MAKE) -C c rebuild
ifneq ($(HAVE_JAVA),)
	$(MAKE) -C java rebuild
endif
