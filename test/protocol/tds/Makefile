top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(FREETDSINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(FREETDSINCLUDES) $(COMPILE) $<

all: tds

tds: tds.cpp tds.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) tds.$(OBJ) $(FREETDSLIBS) $(DROPINTESTLIBS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.class tds$(EXE)
