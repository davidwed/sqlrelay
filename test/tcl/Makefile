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
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" db2.tcl.in > $@
	$(CHMOD) 755 $@

firebird.tcl: firebird.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" firebird.tcl.in > $@
	$(CHMOD) 755 $@

mysql.tcl: mysql.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" mysql.tcl.in > $@
	$(CHMOD) 755 $@

oracle.tcl: oracle.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" oracle.tcl.in > $@
	$(CHMOD) 755 $@

postgresql.tcl: postgresql.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" postgresql.tcl.in > $@
	$(CHMOD) 755 $@

sqlite.tcl: sqlite.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" sqlite.tcl.in > $@
	$(CHMOD) 755 $@

sybase.tcl: sybase.tcl.in
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" sybase.tcl.in > $@
	$(CHMOD) 755 $@

clean:
	$(RM) *.tcl cachefile* sqlnet.log
