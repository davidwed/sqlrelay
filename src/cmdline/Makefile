top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) -DSYSTEM_SQLRSHRC=\"$(sysconfdir)/sqlrsh\" -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
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

all: sqlr-query$(EXE) sqlr-fields$(EXE) sqlrsh$(EXE) sqlr-export$(EXE) sqlr-import$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) sqlr-fields$(EXE)
	$(LIBTOOL) --mode=clean $(RM) sqlr-query$(EXE)
	$(LIBTOOL) --mode=clean $(RM) sqlr-export$(EXE)
	$(LIBTOOL) --mode=clean $(RM) sqlr-import$(EXE)
	$(LIBTOOL) --mode=clean $(RM) sqlrsh$(EXE)
	$(RMTREE) .libs

sqlr-query$(EXE): sqlr-query.cpp sqlr-query.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-query.lo $(LIBS) $(RPATH)

sqlr-export$(EXE): sqlr-export.cpp sqlr-export.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-export.lo $(LIBS) $(RPATH)

sqlr-import$(EXE): sqlr-import.cpp sqlr-import.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-import.lo $(LIBS) $(RPATH)

sqlr-fields$(EXE): sqlr-fields.cpp sqlr-fields.lo
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlr-fields.lo $(LIBS) $(RPATH)

sqlrsh$(EXE): sqlrsh.cpp sqlrsh.lo 
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(CXXFLAGS) $(LDFLAGS) -o $@ sqlrsh.lo $(LIBS) $(READLINELIB) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-query$(EXE) $(bindir)
	$(LTINSTALL) sqlr-fields$(EXE) $(bindir)
	$(LTINSTALL) sqlrsh$(EXE) $(bindir)
	$(LTINSTALL) sqlr-export$(EXE) $(bindir)
	$(LTINSTALL) sqlr-import$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/query$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/fields$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-query$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-fields$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlrsh$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-export$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-import$(EXE)
