top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server $(RUDIMENTSINCLUDES) $(FREETDSINCLUDES)
LIBS = $(FREETDSLIBS) -L$(top_builddir)/src/server -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS) $(PTHREADLIB)
ifneq ($(strip $(FREETDSLIBSPATH)),)
ifeq ($(FREETDSUSERPATH),yes)
	RPATH = -R $(FREETDSLIBSPATH)
endif
endif

SRCS= freetdsconnection.cpp
LOBJS= freetdsconnection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrconnection_freetds.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrconnection_freetds.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrconnection_freetds.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_freetds.a
	$(RM) $(libexecdir)/sqlrconnection_freetds.la
	$(MODULERENAME) $(libexecdir)/sqlrconnection_freetds.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_freetds.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_freetds*
	$(RM) $(bindir)/sqlr-connection-freetds$(EXE)
