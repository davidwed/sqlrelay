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
	$(MAKE) mysql5
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) oracle8i
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql8
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

mysql5: mysql5.C mysql5.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql5.lo $(LIBS)

oracle8i: oracle8i.C oracle8i.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8i.lo $(LIBS)

oracle8iclobinsert: oracle8iclobinsert.C oracle8iclobinsert.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8iclobinsert.lo $(LIBS)

oracle8iclobfetch: oracle8iclobfetch.C oracle8iclobfetch.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8iclobfetch.lo $(LIBS)

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
	$(LIBTOOL) --mode=clean $(RM) mysql5
	$(LIBTOOL) --mode=clean $(RM) oracle8iclobfetch
	$(LIBTOOL) --mode=clean $(RM) oracle8iclobinsert
	$(LIBTOOL) --mode=clean $(RM) oracle8i
	$(LIBTOOL) --mode=clean $(RM) postgresql8
	$(LIBTOOL) --mode=clean $(RM) sqlite
	$(LIBTOOL) --mode=clean $(RM) sybase
	$(LIBTOOL) --mode=clean $(RM) router
	$(RM) cachefile*
	$(RM) sqlnet.log
	$(RMTREE) .libs
