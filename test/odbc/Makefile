top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ODBCTESTCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(ODBCTESTCPPFLAGS) $(COMPILE) $<

all: oracle

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj oracle$(EXE) cachefile* sqlnet.log
	$(RMTREE) .libs

oracle: oracle.cpp oracle.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ oracle.$(OBJ) $(ODBCTESTLIBS)
