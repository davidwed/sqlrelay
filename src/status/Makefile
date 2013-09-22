top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server $(RUDIMENTSINCLUDES) $(STATUSINCLUDES)
LIBS = $(STATUSLIBS) -L$(top_builddir)/src/server -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)

SRCS= sqlr-status.cpp
LOBJS= sqlr-status.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-status$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o sqlr-status$(EXE)
	$(RMTREE) .libs

sqlr-status$(EXE): $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-status$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-status$(EXE)
