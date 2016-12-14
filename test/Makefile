include ../config.mk

all:
	cd c $(AND) $(MAKE) all
	cd c++ $(AND) $(MAKE) all
	cd cs $(AND) $(MAKE) all
	cd java $(AND) $(MAKE) all
	cd stress $(AND) $(MAKE) all
	cd tcl $(AND) $(MAKE) all

clean:
	cd bench $(AND) $(MAKE) clean
	cd c $(AND) $(MAKE) clean
	cd c++ $(AND) $(MAKE) clean
	cd cs $(AND) $(MAKE) clean
	cd dropin $(AND) $(MAKE) clean
	cd java $(AND) $(MAKE) clean
	cd nodejs $(AND) $(MAKE) clean
	cd odbc $(AND) $(MAKE) clean
	cd perl $(AND) $(MAKE) clean
	cd php $(AND) $(MAKE) clean
	cd python $(AND) $(MAKE) clean
	cd ruby $(AND) $(MAKE) clean
	cd stress $(AND) $(MAKE) clean
	cd tcl $(AND) $(MAKE) clean

tests: all
	$(THISDIR)testall$(SCRIPTEXT)
