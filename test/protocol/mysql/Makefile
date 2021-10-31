top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(MYSQLINCLUDES) $(COMPILE) $<

all: mysql bind mysqldatetodatetime

mysql: mysql.cpp mysql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) mysql.$(OBJ) $(MYSQLLIBS) $(DROPINTESTLIBS)

bind: bind.cpp bind.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) bind.$(OBJ) $(MYSQLLIBS) $(DROPINTESTLIBS)

mysqldatetodatetime: mysqldatetodatetime.cpp mysqldatetodatetime.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) mysqldatetodatetime.$(OBJ) $(MYSQLLIBS) $(DROPINTESTLIBS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.class mysql$(EXE) mysqldatetodatetime$(EXE) bind$(EXE)
