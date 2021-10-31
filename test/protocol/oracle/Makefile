top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(ORACLEINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(ORACEINCLUDES) $(COMPILE) $<

.java.class:
	$(JAVAC) $<

all: oracle oracle.class

oracle: oracle.cpp oracle.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracle.$(OBJ) $(ORACLELIBS) $(DROPINTESTLIBS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.class oracle$(EXE)
