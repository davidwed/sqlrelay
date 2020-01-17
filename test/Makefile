top_builddir = ..

include ../config.mk

all: $(TESTALLSUBDIRS) 

all-c:
	cd c $(AND) $(MAKE) all

all-cpp:
	cd c++ $(AND) $(MAKE) all

all-cs:
	cd cs $(AND) $(MAKE) all

all-java:
	cd java $(AND) $(MAKE) all

all-stress:
	cd stress $(AND) $(MAKE) all

all-tcl:
	cd tcl $(AND) $(MAKE) all

all-dropin:
	cd dropin $(AND) $(MAKE) all

clean:
	cd bench $(AND) $(MAKE) clean
	cd c $(AND) $(MAKE) clean
	cd c++ $(AND) $(MAKE) clean
	cd cs $(AND) $(MAKE) clean
	cd dropin $(AND) $(MAKE) clean
	cd java $(AND) $(MAKE) clean
	cd jdbc $(AND) $(MAKE) clean
	cd nodejs $(AND) $(MAKE) clean
	cd odbc $(AND) $(MAKE) clean
	cd perl $(AND) $(MAKE) clean
	cd php $(AND) $(MAKE) clean
	cd python $(AND) $(MAKE) clean
	cd ruby $(AND) $(MAKE) clean
	cd stress $(AND) $(MAKE) clean
	cd tcl $(AND) $(MAKE) clean
	$(RM) sqlr-*.*.bt sybinit.err

tests: all
	$(SCRIPTINT) $(THISDIR)testall$(SCRIPTEXT)
