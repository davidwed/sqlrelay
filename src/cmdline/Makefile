top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CMDLINECPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CMDLINECPPFLAGS) $(COMPILE) $<

all: $(SQLR)sh$(EXE) \
	$(SQLR)-export$(EXE) \
	$(SQLR)-import$(EXE) \
	$(SQLR)-pwdenc$(EXE)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.ii $(SQLR)sh$(EXE) $(SQLR)-export$(EXE) $(SQLR)-import$(EXE) $(SQLR)-pwdenc$(EXE)

$(SQLR)sh$(EXE): sqlrsh.cpp sqlrsh.$(OBJ) 
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlrsh.$(OBJ) $(CMDLINELIBS)

$(SQLR)-export$(EXE): sqlr-export.cpp sqlr-export.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-export.$(OBJ) $(CMDLINELIBS)

$(SQLR)-import$(EXE): sqlr-import.cpp sqlr-import.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-import.$(OBJ) $(CMDLINELIBS)

$(SQLR)-pwdenc$(EXE): sqlr-pwdenc.cpp sqlr-pwdenc.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@ sqlr-pwdenc.$(OBJ) $(CMDLINELIBS)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)sh$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-export$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-import$(EXE) $(bindir)
	$(LTINSTALL) $(CP) $(SQLR)-pwdenc$(EXE) $(bindir)

uninstall:
	$(RM) $(bindir)/$(SQLR)sh$(EXE) \
		$(bindir)/$(SQLR)-export$(EXE) \
		$(bindir)/$(SQLR)-import$(EXE) \
		$(bindir)/$(SQLR)-pwdenc$(EXE) \
		$(bindir)/sqlrsh$(EXE) \
		$(bindir)/sqlr-export$(EXE) \
		$(bindir)/sqlr-import$(EXE) \
		$(bindir)/sqlr-pwdenc$(EXE) \
		$(bindir)/$(SQLR)-fields$(EXE) \
		$(bindir)/$(SQLR)-query$(EXE) \
		$(bindir)/sqlr-query$(EXE) \
		$(bindir)/sqlr-fields$(EXE) \
		$(bindir)/fields$(EXE) \
		$(bindir)/query$(EXE)
