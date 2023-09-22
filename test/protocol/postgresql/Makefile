top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(POSTGRESQLINCLUDES) $(COMPILE) $<

all: postgresql

postgresql: postgresql.cpp postgresql.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) postgresql.$(OBJ) $(POSTGRESQLLIBS) $(RUDIMENTSLIBS)

clean:
	$(RM) *.lo *.o *.obj *.class postgresql$(EXE)
	$(RMTREE) .libs
