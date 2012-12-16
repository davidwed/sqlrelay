top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(DB2INCLUDES) $(SIDINCLUDES)
LIBS = $(DB2LIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(DB2LIBSPATH)),)
ifeq ($(DB2USERPATH),yes)
	RPATH = -R $(DB2LIBSPATH)
endif
endif

SRCS= db2connection.cpp
LOBJS= db2connection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlrconnection_db2.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrconnection_db2.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrconnection_db2.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_db2.a
	$(RM) $(libexecdir)/sqlrconnection_db2.la

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_db2.*
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_db2*
	$(RM) $(bindir)/sqlr-connection-db2$(EXE)
