top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

all: $(TESTPROTOCOLSUBDIRS)

all-mysql: 
	cd mysql $(AND) $(MAKE) all

all-postgresql:
	cd postgresql $(AND) $(MAKE) all

all-tds:
	cd tds $(AND) $(MAKE) all

all-oracle:
	cd oracle $(AND) $(MAKE) all

all-teradata:
	cd teradata $(AND) $(MAKE) all

clean:
	cd mysql $(AND) $(MAKE) clean
	cd postgresql $(AND) $(MAKE) clean
	cd tds $(AND) $(MAKE) clean
	cd oracle $(AND) $(MAKE) clean
	cd teradata $(AND) $(MAKE) clean
