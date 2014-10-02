top_builddir = ../..

include ../../config.mk

.PHONY: all clean

.SUFFIXES: .tcl .tcl.in

.tcl.in.tcl:
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@; chmod 755 $@

all: db2.tcl \
	firebird.tcl \
	mysql.tcl \
	oracle.tcl \
	postgresql.tcl \
	sqlite.tcl \
	sybase.tcl

clean:
	$(RM) *.tcl cachefile* sqlnet.log
