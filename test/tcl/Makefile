top_builddir = ../..

include ../../config.mk

.SUFFIXES: .tcl .in

all: db2.tcl \
	firebird.tcl \
	freetds.tcl \
	mysql.tcl \
	oracle.tcl \
	postgresql.tcl \
	sqlite.tcl \
	sybase.tcl

db2.tcl: db2.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) $@.in > $@.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in > $@.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in
	$(CHMOD) 755 $@

firebird.tcl: firebird.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) $@.in > $@.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in > $@.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in
	$(CHMOD) 755 $@

freetds.tcl: freetds.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) $@.in > $@.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in > $@.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in
	$(CHMOD) 755 $@

mysql.tcl: mysql.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) $@.in > $@.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in > $@.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in
	$(CHMOD) 755 $@

oracle.tcl: oracle.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) $@.in > $@.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in > $@.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in
	$(CHMOD) 755 $@

postgresql.tcl: postgresql.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) $@.in > $@.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in > $@.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in
	$(CHMOD) 755 $@

sqlite.tcl: sqlite.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) $@.in > $@.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in > $@.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in
	$(CHMOD) 755 $@

sybase.tcl: sybase.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) $@.in > $@.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in > $@.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in
	$(CHMOD) 755 $@

clean:
	$(RM) *.in.in *.tcl cachefile* sqlnet.log
