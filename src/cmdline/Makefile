top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CMDLINECPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CMDLINECPPFLAGS) $(COMPILE) $<

all: $(SQLR)-query$(EXE) \
	$(SQLR)-fields$(EXE) \
	$(SQLR)sh$(EXE) \
	$(SQLR)-export$(EXE) \
	$(SQLR)-import$(EXE)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj $(SQLR)-fields$(EXE) $(SQLR)-query$(EXE) $(SQLR)-export$(EXE) $(SQLR)-import$(EXE) $(SQLR)sh$(EXE)

$(SQLR)-fields$(EXE): sqlr-fields.cpp sqlr-fields.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-fields.$(OBJ) $(CMDLINELIBS)

$(SQLR)-query$(EXE): sqlr-query.cpp sqlr-query.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-query.$(OBJ) $(CMDLINELIBS)

$(SQLR)-export$(EXE): sqlr-export.cpp sqlr-export.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-export.$(OBJ) $(CMDLINELIBS)

$(SQLR)-import$(EXE): sqlr-import.cpp sqlr-import.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-import.$(OBJ) $(CMDLINELIBS)

$(SQLR)sh$(EXE): sqlrsh.cpp sqlrsh.$(OBJ) 
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlrsh.$(OBJ) $(CMDLINELIBS) $(READLINELIBS)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-fields$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-query$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-export$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-import$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)sh$(EXE) $(bindir)

uninstall:
	$(RM) $(bindir)/$(SQLR)-fields$(EXE) \
		$(bindir)/$(SQLR)-query$(EXE) \
		$(bindir)/$(SQLR)-export$(EXE) \
		$(bindir)/$(SQLR)-import$(EXE) \
		$(bindir)/$(SQLR)sh$(EXE) \
		$(bindir)/sqlr-fields$(EXE) \
		$(bindir)/sqlr-query$(EXE) \
		$(bindir)/sqlr-export$(EXE) \
		$(bindir)/sqlr-import$(EXE) \
		$(bindir)/sqlrsh$(EXE) \
		$(bindir)/query$(EXE) \
		$(bindir)/fields$(EXE)
