top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CMDLINECPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CMDLINECPPFLAGS) $(COMPILE) $<

all: $(SQLR)sh$(EXE) $(SQLR)-export$(EXE) $(SQLR)-import$(EXE)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.ii $(SQLR)-export$(EXE) $(SQLR)-import$(EXE) $(SQLR)sh$(EXE)

$(SQLR)-export$(EXE): sqlr-export.cpp sqlr-export.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-export.$(OBJ) $(CMDLINELIBS)

$(SQLR)-import$(EXE): sqlr-import.cpp sqlr-import.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-import.$(OBJ) $(CMDLINELIBS)

$(SQLR)sh$(EXE): sqlrsh.cpp sqlrsh.$(OBJ) 
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlrsh.$(OBJ) $(CMDLINELIBS)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)sh$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-export$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-import$(EXE) $(bindir)

uninstall:
	$(RM) $(bindir)/$(SQLR)sh$(EXE) \
		$(bindir)/$(SQLR)-export$(EXE) \
		$(bindir)/$(SQLR)-import$(EXE) \
		$(bindir)/sqlrsh$(EXE) \
		$(bindir)/sqlr-export$(EXE) \
		$(bindir)/sqlr-import$(EXE) \
		$(bindir)/$(SQLR)-fields$(EXE) \
		$(bindir)/$(SQLR)-query$(EXE) \
		$(bindir)/sqlr-query$(EXE) \
		$(bindir)/sqlr-fields$(EXE) \
		$(bindir)/fields$(EXE) \
		$(bindir)/query$(EXE)
