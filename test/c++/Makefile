top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = -L$(libdir) -lsqlrclient $(RUDIMENTSLIBS) -lm $(LIBEFENCE)

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all:
ifneq ($(strip $(DB2LIBS)),)
	$(MAKE) db2
endif
ifneq ($(strip $(FREETDSLIBS)),)
	$(MAKE) freetds
endif
ifneq ($(strip $(FIREBIRDLIBS)),)
	$(MAKE) firebird
endif
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) mysql
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) oracle
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql
endif
ifneq ($(strip $(SQLITELIBS)),)
	$(MAKE) sqlite
endif
ifneq ($(strip $(SYBASELIBS)),)
	$(MAKE) sybase
endif
ifneq ($(ENABLE_ROUTER)),)
	$(MAKE) router
endif

db2: db2.C db2.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db2.lo $(LIBS)

freetds: freetds.C freetds.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ freetds.lo $(LIBS)

firebird: firebird.C firebird.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ firebird.lo $(LIBS)

mysql: mysql.C mysql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql.lo $(LIBS)

oracle: oracle.C oracle.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle.lo $(LIBS)

oracleclobinsert: oracleclobinsert.C oracleclobinsert.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracleclobinsert.lo $(LIBS)

oracleclobfetch: oracleclobfetch.C oracleclobfetch.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracleclobfetch.lo $(LIBS)

sqlite: sqlite.C sqlite.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlite.lo $(LIBS)

sybase: sybase.C sybase.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sybase.lo $(LIBS)

router: router.C router.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ router.lo $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) db2
	$(LIBTOOL) --mode=clean $(RM) freetds
	$(LIBTOOL) --mode=clean $(RM) firebird
	$(LIBTOOL) --mode=clean $(RM) mysql
	$(LIBTOOL) --mode=clean $(RM) oracleclobfetch
	$(LIBTOOL) --mode=clean $(RM) oracleclobinsert
	$(LIBTOOL) --mode=clean $(RM) oracle
	$(LIBTOOL) --mode=clean $(RM) postgresql
	$(LIBTOOL) --mode=clean $(RM) sqlite
	$(LIBTOOL) --mode=clean $(RM) sybase
	$(LIBTOOL) --mode=clean $(RM) router
	$(RM) cachefile*
	$(RM) sqlnet.log
	$(RMTREE) .libs
