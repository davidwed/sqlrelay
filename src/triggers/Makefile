top_builddir = ../..

include ../../config.mk

LIBS = -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS)

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/ -I$(top_builddir)/src/server/include -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqlrtrigger_createtableautoincrementoracle.la \
	sqlrtrigger_droptableautoincrementoracle.la \
	sqlrtrigger_droplocalizedtemptables.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqlrtrigger_createtableautoincrementoracle.la: createtableautoincrementoracle.cpp createtableautoincrementoracle.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ createtableautoincrementoracle.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtrigger_droptableautoincrementoracle.la: droptableautoincrementoracle.cpp droptableautoincrementoracle.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ droptableautoincrementoracle.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqlrtrigger_droplocalizedtemptables.la: droplocalizedtemptables.cpp droplocalizedtemptables.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ droplocalizedtemptables.lo $(LDFLAGS) $(LIBS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqlrtrigger_createtableautoincrementoracle.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtrigger_createtableautoincrementoracle.a
	$(RM) $(libexecdir)/sqlrtrigger_createtableautoincrementoracle.la
	$(MODULERENAME) $(libexecdir)/sqlrtrigger_createtableautoincrementoracle.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtrigger_droptableautoincrementoracle.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtrigger_droptableautoincrementoracle.a
	$(RM) $(libexecdir)/sqlrtrigger_droptableautoincrementoracle.la
	$(MODULERENAME) $(libexecdir)/sqlrtrigger_droptableautoincrementoracle.so so $(MODULESUFFIX)
	$(LTINSTALL) sqlrtrigger_droplocalizedtemptables.la $(libexecdir)
	$(RM) $(libexecdir)/sqlrtrigger_droplocalizedtemptables.a
	$(RM) $(libexecdir)/sqlrtrigger_droplocalizedtemptables.la
	$(MODULERENAME) $(libexecdir)/sqlrtrigger_droplocalizedtemptables.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqltrigger_*
	$(RM) $(libexecdir)/sqlrtrigger_*
