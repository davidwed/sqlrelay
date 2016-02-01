top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(STRESSCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(STRESSCPPFLAGS) $(COMPILE) $<

all: socketeater stress connectdisconnect

socketeater: socketeater.cpp socketeater.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) socketeater.$(OBJ) $(STRESSLIBS)

stress: stress.cpp stress.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) stress.$(OBJ) $(STRESSLIBS)

connectdisconnect: connectdisconnect.cpp connectdisconnect.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) connectdisconnect.$(OBJ) $(STRESSLIBS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.lib *.exp *.pdb *.manifest socketeater$(EXE) stress$(EXE) connectdisconnect$(EXE)
	$(RMTREE) .libs
