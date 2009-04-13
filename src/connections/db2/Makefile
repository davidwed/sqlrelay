top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(DB2INCLUDES) $(SIDINCLUDES)
LIBS = $(DB2LIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(DB2LIBSPATH)),)
ifeq ($(DB2USERPATH),yes)
	RPATH = -R $(DB2LIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= db2connection.C main.C
LOBJS= db2connection.lo main.lo
DLOBJS= db2connection.d.lo main.d.lo

.SUFFIXES: .lo .d.lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-db2$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-db2$(EXE)
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

rebuild: clean all

sqlr-connection-db2$(EXE): $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(DB2STATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection-db2$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-db2$(EXE)
