top_builddir = ../..

include ../../config.mk

.SUFFIXES: .tcl .in

.in.tcl:
	sed -e "s|@TCLLIBSPATH@|$(TCLLIBSPATH)|g" $< > $@; $(CHMOD) 755 $@

all: db2.tcl \
	firebird.tcl \
	mysql.tcl \
	oracle.tcl \
	postgresql.tcl \
	sqlite.tcl \
	sybase.tcl

clean:
	$(RM) *.tcl cachefile* sqlnet.log
