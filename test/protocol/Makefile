top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

all:
	cd mysql $(AND) $(MAKE) all
	cd oracle $(AND) $(MAKE) all
	cd postgresql $(AND) $(MAKE) all
	cd tds $(AND) $(MAKE) all
	cd teradata $(AND) $(MAKE) all

clean:
	cd mysql $(AND) $(MAKE) clean
	cd oracle $(AND) $(MAKE) clean
	cd postgresql $(AND) $(MAKE) clean
	cd tds $(AND) $(MAKE) clean
	cd teradata $(AND) $(MAKE) clean
