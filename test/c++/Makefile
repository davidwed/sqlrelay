top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = -L$(libdir) -lsqlrclient $(RUDIMENTSLIBS) $(LIBEFENCE)

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all:
ifneq ($(strip $(DB2LIBS)),)
	$(MAKE) db2
	$(MAKE) db27
	$(MAKE) db26
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
	$(MAKE) oracle8
	$(MAKE) oracle7
	$(MAKE) extensions
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

db2: db2.cpp db2.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db2.lo $(LIBS)

db27: db27.cpp db27.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db27.lo $(LIBS)

db26: db26.cpp db26.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db26.lo $(LIBS)

freetds: freetds.cpp freetds.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ freetds.lo $(LIBS)

firebird: firebird.cpp firebird.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ firebird.lo $(LIBS)

mysql: mysql.cpp mysql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql.lo $(LIBS)

oracle: oracle.cpp oracle.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle.lo $(LIBS)

oracle8: oracle8.cpp oracle8.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8.lo $(LIBS)
oracle7: oracle7.cpp oracle7.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle7.lo $(LIBS)

extensions: extensions.cpp extensions.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ extensions.lo $(LIBS)

postgresql: postgresql.cpp postgresql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql.lo $(LIBS)

oracleclobinsert: oracleclobinsert.cpp oracleclobinsert.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracleclobinsert.lo $(LIBS)

oracleclobfetch: oracleclobfetch.cpp oracleclobfetch.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracleclobfetch.lo $(LIBS)

sqlite: sqlite.cpp sqlite.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlite.lo $(LIBS)

sybase: sybase.cpp sybase.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sybase.lo $(LIBS)

router: router.cpp router.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ router.lo $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o db2 db27 db26 extensions freetds firebird mysql oracleclobfetch oracleclobinsert oracle oracle8 oracle7 postgresql sqlite sybase router
	$(RM) cachefile* sqlnet.log
	$(RMTREE) .libs
