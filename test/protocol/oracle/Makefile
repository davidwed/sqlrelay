top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(ORACLEINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(ORACEINCLUDES) $(COMPILE) $<

all: oracle oracle.class

oracle.class: oracle.java
	$(JAVAC) $<

oracle: oracle.cpp oracle.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) oracle.$(OBJ) $(ORACLELIBS) $(RUDIMENTSLIBS)

clean:
	$(RM) *.lo *.o *.obj *.class oracle$(EXE)
	$(RMTREE) .libs
