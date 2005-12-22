top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/authenticator -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(FREETDSINCLUDES) $(SIDINCLUDES)
LIBS = $(FREETDSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(FREETDSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection_debug -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(FREETDSLIBSPATH)),)
ifeq ($(FREETDSUSERPATH),yes)
	RPATH = -R $(FREETDSLIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= freetdsconnection.C main.C
LOBJS= freetdsconnection.lo main.lo
DLOBJS= freetdsconnection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.d.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -DSERVER_DEBUG -c $< -o $@

all: sqlr-connection-freetds sqlr-connection-freetds-debug

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-freetds
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-freetds-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

rebuild: clean all

sqlr-connection-freetds: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(FREETDSSTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

sqlr-connection-freetds-debug: $(SRCS) $(DLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(FREETDSSTATIC) $(LDFLAGS) -o $@ $(DLOBJS) $(DEBUGLIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection-freetds$(EXE) $(bindir)
	$(LTINSTALL) sqlr-connection-freetds-debug$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-freetds$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-freetds-debug$(EXE)
