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
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@
	$(CHMOD) 755 $@

firebird.tcl: firebird.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@
	$(CHMOD) 755 $@

mysql.tcl: mysql.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@
	$(CHMOD) 755 $@

oracle.tcl: oracle.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@
	$(CHMOD) 755 $@

postgresql.tcl: postgresql.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@
	$(CHMOD) 755 $@

sqlite.tcl: sqlite.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@
	$(CHMOD) 755 $@

sybase.tcl: sybase.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@
	$(CHMOD) 755 $@

clean:
	$(RM) *.tcl cachefile* sqlnet.log
