top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(STATUSINCLUDES) $(SIDINCLUDES)
LIBS = $(STATUSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(STATUSLIBSPATH)),)
ifeq ($(STATUSUSERPATH),yes)
	RPATH = -R $(STATUSLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= statusconnection.C main.C
LOBJS= statusconnection.lo main.lo
DLOBJS= statusconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-status$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-status$(EXE)
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

rebuild: clean all

sqlr-status$(EXE): $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATUSSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-status$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-status$(EXE)
