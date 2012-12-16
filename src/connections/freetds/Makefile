top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/authenticator -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(FREETDSINCLUDES) $(SIDINCLUDES)
LIBS = $(FREETDSLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(PTHREADLIB) $(LIBDMALLOC) $(LIBEFENCE)
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
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrconnection_freetds.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_freetds.a
	$(RM) $(libexecdir)/sqlrconnection_freetds.la

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_freetds.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_freetds*
	$(RM) $(bindir)/sqlr-connection-freetds$(EXE)
