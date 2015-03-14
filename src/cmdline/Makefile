top_builddir = ../..

include ../../config.mk

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) -DSYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" $(CACHEDIRCPPFLAG) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(READLINEINCLUDES)
LIBS = -L$(top_builddir)/src/util -lsqlrutil -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(CLIENTUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

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
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-fields.$(OBJ) $(LIBS) $(RPATH)

sqlr-query$(EXE): sqlr-query.cpp sqlr-query.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-query.$(OBJ) $(LIBS) $(RPATH)

sqlr-export$(EXE): sqlr-export.cpp sqlr-export.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-export.$(OBJ) $(LIBS) $(RPATH)

sqlr-import$(EXE): sqlr-import.cpp sqlr-import.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-import.$(OBJ) $(LIBS) $(RPATH)

sqlr-pwdenc$(EXE): sqlr-pwdenc.cpp sqlr-pwdenc.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-pwdenc.$(OBJ) -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE) $(RPATH)

sqlr-status$(EXE): sqlr-status.cpp sqlr-status.$(OBJ)
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlr-status.$(OBJ) $(LIBS) $(RPATH)

sqlrsh$(EXE): sqlrsh.cpp sqlrsh.$(OBJ) 
	$(LTLINK) $(LINK) $(CXXFLAGS) $(LDFLAGS) $(OUT)$@ sqlrsh.$(OBJ) $(LIBS) $(READLINELIBS) $(RPATH)

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
