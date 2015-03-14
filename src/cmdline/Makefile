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
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-query$(EXE) \
	sqlr-fields$(EXE) \
	sqlrsh$(EXE) \
	sqlr-export$(EXE) \
	sqlr-import$(EXE) \
	sqlr-pwdenc$(EXE) \
	sqlr-status$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o sqlr-fields$(EXE) sqlr-query$(EXE) sqlr-export$(EXE) sqlr-import$(EXE) sqlr-pwdenc$(EXE) sqlr-status$(EXE) sqlrsh$(EXE)
	$(RMTREE) .libs

sqlr-fields$(EXE): sqlr-fields.cpp sqlr-fields.lo
	$(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-fields.lo $(LIBS) $(RPATH)

sqlr-query$(EXE): sqlr-query.cpp sqlr-query.lo
	$(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-query.lo $(LIBS) $(RPATH)

sqlr-export$(EXE): sqlr-export.cpp sqlr-export.lo
	$(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-export.lo $(LIBS) $(RPATH)

sqlr-import$(EXE): sqlr-import.cpp sqlr-import.lo
	$(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-import.lo $(LIBS) $(RPATH)

sqlr-pwdenc$(EXE): sqlr-pwdenc.cpp sqlr-pwdenc.lo
	$(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-pwdenc.lo -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE) $(RPATH)

sqlr-status$(EXE): sqlr-status.cpp sqlr-status.lo
	$(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-status.lo $(LIBS) $(RPATH)

sqlrsh$(EXE): sqlrsh.cpp sqlrsh.lo 
	$(LIBTOOL) --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.lo $(LIBS) $(READLINELIBS) $(RPATH)

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
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/query$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/fields$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-fields$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-query$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-export$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-import$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-pwdenc$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-status$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlrsh$(EXE)
