top_builddir = ../..

include ../../config.mk

.SUFFIXES: .tcl .in

all: db2.tcl \
	firebird.tcl \
	mysql.tcl \
	oracle.tcl \
	postgresql.tcl \
	sqlite.tcl \
	sybase.tcl

db2.tcl: db2.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) db2.tcl.in > $@.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in > $@
	$(RM) $@.in.in
	$(CHMOD) 755 $@

firebird.tcl: firebird.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) firebird.tcl.in > $@.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in > $@
	$(RM) $@.in.in
	$(CHMOD) 755 $@

mysql.tcl: mysql.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) mysql.tcl.in > $@.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in > $@
	$(RM) $@.in.in
	$(CHMOD) 755 $@

oracle.tcl: oracle.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) oracle.tcl.in > $@.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in > $@
	$(RM) $@.in.in
	$(CHMOD) 755 $@

postgresql.tcl: postgresql.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) postgresql.tcl.in > $@.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in > $@
	$(RM) $@.in.in
	$(CHMOD) 755 $@

sqlite.tcl: sqlite.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) sqlite.tcl.in > $@.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in > $@
	$(RM) $@.in.in
	$(CHMOD) 755 $@

sybase.tcl: sybase.tcl.in
	$(REPLACE) @TCLLIBSPATH@ $(TCLLIBSPATH) sybase.tcl.in > $@.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in > $@
	$(RM) $@.in.in
	$(CHMOD) 755 $@

clean:
	$(RM) *.in.in *.tcl cachefile* sqlnet.log
