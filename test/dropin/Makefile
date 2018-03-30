top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

all: $(DROPINTESTTARGETS)

mysql.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) $< $(OUT)$@

bind.lo: bind.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) $< $(OUT)$@

mysql.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) mysql.cpp 

mysqldatetodatetime.lo: mysqldatetodatetime.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) $< $(OUT)$@

mysqldatetodatetime.obj: mysqldatetodatetime.cpp
	$(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) mysqldatetodatetime.cpp 

postgresql.lo: postgresql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) $< $(OUT)$@

postgresql.obj: postgresql.cpp
	$(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) postgresql.cpp

tds.lo: tds.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(FREETDSINCLUDES) $(COMPILE) $< $(OUT)$@

tds.obj: tds.cpp
	$(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(FREETDSINCLUDES) $(COMPILE) tds.cpp

oracle.lo: oracle.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(ORACLEINCLUDES) $(COMPILE) $< $(OUT)$@

oracle.obj: oracle.cpp
	$(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(ORACLENCLUDES) $(COMPILE) tds.cpp

oracle.class: oracle.java
	$(JAVAC) oracle.java

odbc.class: odbc.java
	$(JAVAC) odbc.java

mysql: mysql.cpp mysql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) mysql.$(OBJ) $(MYSQLLIBS) $(DROPINTESTLIBS)

bind: bind.cpp bind.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) bind.$(OBJ) $(MYSQLLIBS) $(DROPINTESTLIBS)

mysqldatetodatetime: mysqldatetodatetime.cpp mysqldatetodatetime.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) mysqldatetodatetime.$(OBJ) $(MYSQLLIBS) $(DROPINTESTLIBS)

postgresql: postgresql.cpp postgresql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) postgresql.$(OBJ) $(POSTGRESQLLIBS) $(DROPINTESTLIBS)

tds: tds.cpp tds.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) tds.$(OBJ) $(FREETDSLIBS) $(DROPINTESTLIBS)

oracle: oracle.cpp oracle.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracle.$(OBJ) $(ORACLELIBS) $(DROPINTESTLIBS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.class mysql$(EXE) postgresql$(EXE) mysqldatetodatetime$(EXE) tds$(EXE) oracle$(EXE) bind$(EXE)
