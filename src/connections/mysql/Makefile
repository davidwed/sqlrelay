top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(MYSQLINCLUDES) $(SIDINCLUDES)
LIBS = $(MYSQLLIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(SIDLIBS) $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(MYSQLLIBSPATH)),)
ifeq ($(MYSQLUSERPATH),yes)
	RPATH = -R $(MYSQLLIBSPATH)
endif
endif

SRCS= mysqlconnection.cpp mysqlsqlwriter.cpp
LOBJS= mysqlconnection.lo mysqlsqlwriter.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrelay_sqlrconnection_mysql.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

libsqlrelay_sqlrconnection_mysql.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrelay_sqlrconnection_mysql.la $(libdir)
	$(RM) $(libdir)/libsqlrelay_sqlrconnection_mysql.a

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrelay_sqlrconnection_mysql.la
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-mysql$(EXE)
