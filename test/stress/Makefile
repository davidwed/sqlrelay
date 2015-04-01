top_builddir = ../..

include ../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = -L$(libdir) -lsqlrclient $(RUDIMENTSLIBS) $(LIBEFENCE)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: cursortest forktest query random socketeater threadtest timeouttest

cursortest: cursortest.cpp cursortest.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) cursortest.$(OBJ) $(LIBS)

forktest: forktest.cpp forktest.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) forktest.$(OBJ) $(LIBS)

query: query.cpp query.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) query.$(OBJ) $(LIBS)

random: random.cpp random.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) random.$(OBJ) $(LIBS)

socketeater: socketeater.cpp socketeater.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) socketeater.$(OBJ) $(LIBS)

threadtest: threadtest.cpp threadtest.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) threadtest.$(OBJ) $(LIBS)

timeouttest: timeouttest.cpp timeouttest.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) timeouttest.$(OBJ) $(LIBS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.lib *.exp *.pdb *.manifest cursortest$(EXE) forktest$(EXE) query$(EXE) random$(EXE) socketeater$(EXE) threadtest$(EXE) timeouttest$(EXE) cachefile*
	$(RMTREE) .libs
