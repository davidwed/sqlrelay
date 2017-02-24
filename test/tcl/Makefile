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
	sap.tcl \
	tls.tcl \
	krb.tcl

db2.tcl: db2.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

firebird.tcl: firebird.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

freetds.tcl: freetds.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

mysql.tcl: mysql.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

oracle.tcl: oracle.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

postgresql.tcl: postgresql.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

sqlite.tcl: sqlite.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

sap.tcl: sap.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

tls.tcl: tls.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

krb.tcl: krb.tcl.in
	$(REPLACE) @TCLLIBSPATH@ "$(TCLLIBSPATH)" $@.in > $@.in.in.in.in
	$(REPLACE) @SOSUFFIX@ $(SOSUFFIX) $@.in.in.in.in > $@.in.in.in
	$(REPLACE) @SLASH@ $(SLASH) $@.in.in.in > $@.in.in
	$(REPLACE) C:\Tcl\lib C:\\Tcl\\lib $@.in.in > $@
	$(RM) $@.in.in $@.in.in.in $@.in.in.in.in
	$(CHMOD) 755 $@

clean:
	$(RM) *.in.in *.in.in.in *.in.in.in.in *.tcl cachefile* sqlnet.log
