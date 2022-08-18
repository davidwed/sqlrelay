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

all-protocol:
	cd protocol $(AND) $(MAKE) all

all-crud:
	cd crud $(AND) $(MAKE) all

clean:
	cd bench $(AND) $(MAKE) clean
	cd c $(AND) $(MAKE) clean
	cd c++ $(AND) $(MAKE) clean
	cd cs $(AND) $(MAKE) clean
	cd protocol $(AND) $(MAKE) clean
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
	cd crud $(AND) $(MAKE) clean
	$(RM) sqlr-*.*.bt sybinit.err log/*.log sqlrelay.conf.d/sqlite/sqlite.db temp1 temp2 temp3

tests: all
	$(SCRIPTINT) $(THISDIR)testall$(SCRIPTEXT)

install:
	$(MKINSTALLDIRS) $(sysconfdir)/sqlrelay.conf.d
	$(MKINSTALLDIRS) $(sysconfdir)/sqlrelay.conf.d/freetds/etc
	$(MKINSTALLDIRS) $(sysconfdir)/sqlrelay.conf.d/pwdenc
	$(MKINSTALLDIRS) $(sysconfdir)/sqlrelay.conf.d/sqlite
	$(MKINSTALLDIRS) $(sysconfdir)/sqlrelay.conf.d/tls
	# FIXME: implement for windows
	for file in `ls sqlrelay.conf.d/*.in`; \
	do \
		$(REPLACE) @HOSTNAME@ testdb $$file > temp1; \
		$(REPLACE) @SHORTHOSTNAME@ testdb temp1 > temp2; \
		$(REPLACE) @abs_top_builddir@/test $(sysconfdir) temp2 > temp3; \
		$(CP) temp3 $(sysconfdir)/sqlrelay.conf.d/`basename $$file .in`; \
		$(RM) temp1 temp2 temp3; \
	done
	$(CP) sqlrelay.conf.d/freetds/etc/* $(sysconfdir)/sqlrelay.conf.d/freetds/etc
	$(CP) sqlrelay.conf.d/pwdenc/* $(sysconfdir)/sqlrelay.conf.d/pwdenc
	$(CP) sqlrelay.conf.d/tls/* $(sysconfdir)/sqlrelay.conf.d/tls
