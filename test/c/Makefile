top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = -L$(libdir) -lsqlrclientwrapper -lsqlrclient $(RUDIMENTSLIBS) -lm $(LIBEFENCE)

.SUFFIXES: .lo

.c.lo:
	$(LIBTOOLGCC) --mode=compile $(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

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
	$(MAKE) oracle8
	$(MAKE) oracle8i
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql6
	$(MAKE) postgresql7
endif
ifneq ($(strip $(SQLITELIBS)),)
	$(MAKE) sqlite
endif
ifneq ($(strip $(SYBASELIBS)),)
	$(MAKE) sybase
endif

db2: db2.c db2.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ db2.lo $(LIBS)

freetds: freetds.c freetds.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ freetds.lo $(LIBS)

firebird: firebird.c firebird.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ firebird.lo $(LIBS)

mysql: mysql.c mysql.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ mysql.lo $(LIBS)

oracle8: oracle8.c oracle8.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ oracle8.lo $(LIBS)

oracle8i: oracle8i.c oracle8i.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ oracle8i.lo $(LIBS)

postgresql6: postgresql6.c postgresql6.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ postgresql6.lo $(LIBS)

postgresql7: postgresql7.c postgresql7.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ postgresql7.lo $(LIBS)

sqlite: sqlite.c sqlite.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ sqlite.lo $(LIBS)

sybase: sybase.c sybase.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ sybase.lo $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) db2
	$(LIBTOOL) --mode=clean $(RM) freetds
	$(LIBTOOL) --mode=clean $(RM) firebird
	$(LIBTOOL) --mode=clean $(RM) mysql
	$(LIBTOOL) --mode=clean $(RM) oracle8
	$(LIBTOOL) --mode=clean $(RM) oracle8i
	$(LIBTOOL) --mode=clean $(RM) postgresql6
	$(LIBTOOL) --mode=clean $(RM) postgresql7
	$(LIBTOOL) --mode=clean $(RM) sqlite
	$(LIBTOOL) --mode=clean $(RM) sybase
	$(RM) cachefile*
	$(RM) sqlnet.log
	$(RMTREE) .libs
