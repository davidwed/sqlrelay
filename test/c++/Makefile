top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(COMPILE) $<

all: db2 \
	db27 \
	db26 \
	freetds \
	firebird \
	informix \
	mysql \
	oracle \
	oracle8 \
	oracle7 \
	postgresql \
	sqlite \
	sap \
	router \
	extensions \
	krb \
	tls

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj db2$(EXE) db27$(EXE) db26$(EXE) freetds$(EXE) firebird$(EXE) informix$(EXE) mysql$(EXE) oracleclobfetch$(EXE) oracleclobinsert$(EXE) oracle$(EXE) oracle8$(EXE) oracle7$(EXE) postgresql$(EXE) sqlite$(EXE) sap$(EXE) router$(EXE) extensions$(EXE) krb$(EXE) tls$(EXE) deadlockreplay$(EXE) cachefile* sqlnet.log
	$(RMTREE) .libs

db2: db2.cpp db2.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) db2.$(OBJ) $(CPPTESTLIBS)

db27: db27.cpp db27.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) db27.$(OBJ) $(CPPTESTLIBS)

db26: db26.cpp db26.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) db26.$(OBJ) $(CPPTESTLIBS)

freetds: freetds.cpp freetds.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) freetds.$(OBJ) $(CPPTESTLIBS)

firebird: firebird.cpp firebird.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) firebird.$(OBJ) $(CPPTESTLIBS)

informix: informix.cpp informix.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) informix.$(OBJ) $(CPPTESTLIBS)

mysql: mysql.cpp mysql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) mysql.$(OBJ) $(CPPTESTLIBS)

oracle: oracle.cpp oracle.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracle.$(OBJ) $(CPPTESTLIBS)

oracle8: oracle8.cpp oracle8.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracle8.$(OBJ) $(CPPTESTLIBS)
oracle7: oracle7.cpp oracle7.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracle7.$(OBJ) $(CPPTESTLIBS)

postgresql: postgresql.cpp postgresql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) postgresql.$(OBJ) $(CPPTESTLIBS)

oracleclobinsert: oracleclobinsert.cpp oracleclobinsert.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracleclobinsert.$(OBJ) $(CPPTESTLIBS)

oracleclobfetch: oracleclobfetch.cpp oracleclobfetch.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracleclobfetch.$(OBJ) $(CPPTESTLIBS)

sqlite: sqlite.cpp sqlite.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) sqlite.$(OBJ) $(CPPTESTLIBS)

sap: sap.cpp sap.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) sap.$(OBJ) $(CPPTESTLIBS)

router: router.cpp router.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) router.$(OBJ) $(CPPTESTLIBS)

extensions: extensions.cpp extensions.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) extensions.$(OBJ) $(CPPTESTLIBS)

krb: krb.cpp krb.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) krb.$(OBJ) $(CPPTESTLIBS)

tls: tls.cpp tls.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) tls.$(OBJ) $(CPPTESTLIBS)

deadlockreplay: deadlockreplay.cpp deadlockreplay.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) deadlockreplay.$(OBJ) $(CPPTESTLIBS)
