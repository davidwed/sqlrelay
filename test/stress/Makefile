top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(STRESSCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(STRESSCPPFLAGS) $(COMPILE) $<

all: socketeater stress testtable churn

socketeater: socketeater.cpp socketeater.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) socketeater.$(OBJ) $(STRESSLIBS)

stress: stress.cpp stress.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) stress.$(OBJ) $(STRESSLIBS)

testtable: testtable.cpp testtable.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) testtable.$(OBJ) $(STRESSLIBS)

churn: churn.cpp churn.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) churn.$(OBJ) $(STRESSLIBS)

clean:
	$(RM) *.lo *.o *.obj *.lib *.exp *.pdb *.manifest socketeater$(EXE) stress$(EXE) churn$(EXE) testtable$(EXE)
	$(RMTREE) .libs
