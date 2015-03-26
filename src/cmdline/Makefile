top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CMDLINECPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CMDLINECPPFLAGS) $(COMPILE) $<

all: sqlr-query$(EXE) \
	sqlr-fields$(EXE) \
	sqlrsh$(EXE) \
	sqlr-export$(EXE) \
	sqlr-import$(EXE) \
	sqlr-pwdenc$(EXE) \
	sqlr-status$(EXE)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj sqlr-fields$(EXE) sqlr-query$(EXE) sqlr-export$(EXE) sqlr-import$(EXE) sqlr-pwdenc$(EXE) sqlr-status$(EXE) sqlrsh$(EXE)

sqlr-fields$(EXE): sqlr-fields.cpp sqlr-fields.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-fields.$(OBJ) $(CMDLINELIBS)

sqlr-query$(EXE): sqlr-query.cpp sqlr-query.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-query.$(OBJ) $(CMDLINELIBS)

sqlr-export$(EXE): sqlr-export.cpp sqlr-export.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-export.$(OBJ) $(CMDLINELIBS)

sqlr-import$(EXE): sqlr-import.cpp sqlr-import.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-import.$(OBJ) $(CMDLINELIBS)

sqlr-pwdenc$(EXE): sqlr-pwdenc.cpp sqlr-pwdenc.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-pwdenc.$(OBJ) $(PWDENCLIBS)

sqlr-status$(EXE): sqlr-status.cpp sqlr-status.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-status.$(OBJ) $(CMDLINELIBS)

sqlrsh$(EXE): sqlrsh.cpp sqlrsh.$(OBJ) 
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlrsh.$(OBJ) $(CMDLINELIBS) $(READLINELIBS)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) sqlr-fields$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-query$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-export$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-import$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-pwdenc$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlr-status$(EXE) $(bindir)
	$(LTINSTALL) $(CP) sqlrsh$(EXE) $(bindir)

uninstall:
	$(LTUNINSTALL) $(RM) $(bindir)/query$(EXE)
	$(LTUNINSTALL) $(RM) $(bindir)/fields$(EXE)
	$(LTUNINSTALL) $(RM) $(bindir)/sqlr-fields$(EXE)
	$(LTUNINSTALL) $(RM) $(bindir)/sqlr-query$(EXE)
	$(LTUNINSTALL) $(RM) $(bindir)/sqlr-export$(EXE)
	$(LTUNINSTALL) $(RM) $(bindir)/sqlr-import$(EXE)
	$(LTUNINSTALL) $(RM) $(bindir)/sqlr-pwdenc$(EXE)
	$(LTUNINSTALL) $(RM) $(bindir)/sqlr-status$(EXE)
	$(LTUNINSTALL) $(RM) $(bindir)/sqlrsh$(EXE)
