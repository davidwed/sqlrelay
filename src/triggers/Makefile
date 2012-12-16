top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(WERROR) $(BASECPPFLAGS) -I$(top_builddir)/ -I$(top_builddir)/src/connection -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -c $< -o $@

all: sqltrigger_createtableautoincrementoracle.la \
	sqltrigger_droptableautoincrementoracle.la \
	sqltrigger_droplocalizedtemptables.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o
	$(RMTREE) .libs

sqltrigger_createtableautoincrementoracle.la: createtableautoincrementoracle.cpp createtableautoincrementoracle.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ createtableautoincrementoracle.lo $(LDFLAGS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltrigger_droptableautoincrementoracle.la: droptableautoincrementoracle.cpp droptableautoincrementoracle.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ droptableautoincrementoracle.lo $(LDFLAGS) -rpath $(libexecdir) -avoid-version -module -no-undefined

sqltrigger_droplocalizedtemptables.la: droplocalizedtemptables.cpp droplocalizedtemptables.lo
	$(LIBTOOL) --mode=link $(CXX) -o $@ droplocalizedtemptables.lo $(LDFLAGS) -rpath $(libexecdir) -avoid-version -module -no-undefined

install:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) sqltrigger_createtableautoincrementoracle.la $(libexecdir)
	$(RM) $(libexecdir)/sqltrigger_createtableautoincrementoracle.a
	$(RM) $(libexecdir)/sqltrigger_createtableautoincrementoracle.la
	$(LTINSTALL) sqltrigger_droptableautoincrementoracle.la $(libexecdir)
	$(RM) $(libexecdir)/sqltrigger_droptableautoincrementoracle.a
	$(RM) $(libexecdir)/sqltrigger_droptableautoincrementoracle.la
	$(LTINSTALL) sqltrigger_droplocalizedtemptables.la $(libexecdir)
	$(RM) $(libexecdir)/sqltrigger_droplocalizedtemptables.a
	$(RM) $(libexecdir)/sqltrigger_droplocalizedtemptables.la
	$(LIBTOOL) --mode=finish $(libexecdir)

uninstall:
	$(RM) $(libexecdir)/sqltrigger_*
