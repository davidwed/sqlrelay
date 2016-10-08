top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

all: $(DROPINTESTTARGETS)

mysql.lo: mysql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) $< $(OUT)$@

mysql.obj: mysql.cpp
	$(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) mysql.cpp 

postgresql.lo: postgresql.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) $< $(OUT)$@

postgresql.obj: postgresql.cpp
	$(CXX) $(CXXFLAGS) $(DROPINTESTCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) postgresql.cpp

mysql: mysql.cpp mysql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) mysql.$(OBJ) $(MYSQLLIBS) $(DROPINTESTLIBS)

postgresql: postgresql.cpp postgresql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) postgresql.$(OBJ) $(POSTGRESQLLIBS) $(DROPINTESTLIBS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj mysql$(EXE) postgresql$(EXE)
