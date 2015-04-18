top_builddir = ../..

include ../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = -L$(libdir) -lsqlrclient $(RUDIMENTSLIBS) $(LIBEFENCE)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: random socketeater stress

random: random.cpp random.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) random.$(OBJ) $(LIBS)

socketeater: socketeater.cpp socketeater.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) socketeater.$(OBJ) $(LIBS)

stress: stress.cpp stress.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) stress.$(OBJ) $(LIBS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.lib *.exp *.pdb *.manifest random$(EXE) socketeater$(EXE) stress$(EXE)
	$(RMTREE) .libs
