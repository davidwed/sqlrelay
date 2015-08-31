top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.c.lo:
	$(LTCOMPILE) $(CC) $(CFLAGS) $(CTESTCPPFLAGS) $(COMPILE) $< $(OUT)$@

.c.obj:
	$(CC) $(CFLAGS) $(CTESTCPPFLAGS) $(COMPILE) $<

all: db2 freetds firebird mysql oracle postgresql sqlite sap

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj db2$(EXE) freetds$(EXE) firebird$(EXE) mysql$(EXE) oracle$(EXE) postgresql$(EXE) sqlite$(EXE) sap$(EXE) cachefile* sqlnet.log
	$(RMTREE) .libs

db2: db2.c db2.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) db2.$(OBJ) $(CTESTLIBS)

freetds: freetds.c freetds.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) freetds.$(OBJ) $(CTESTLIBS)

firebird: firebird.c firebird.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) firebird.$(OBJ) $(CTESTLIBS)

mysql: mysql.c mysql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) mysql.$(OBJ) $(CTESTLIBS)

oracle: oracle.c oracle.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracle.$(OBJ) $(CTESTLIBS)

postgresql: postgresql.c postgresql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) postgresql.$(OBJ) $(CTESTLIBS)

sqlite: sqlite.c sqlite.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) sqlite.$(OBJ) $(CTESTLIBS)

sap: sap.c sap.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) sap.$(OBJ) $(CTESTLIBS)
