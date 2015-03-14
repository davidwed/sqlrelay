top_builddir = ../..

include ../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = -L$(libdir) -lsqlrclientwrapper -lsqlrclient $(RUDIMENTSLIBS) $(LIBEFENCE)

.SUFFIXES: .lo

.c.lo:
	$(LIBTOOLGCC) --mode=compile $(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

all:
	$(MAKE) db2
	$(MAKE) freetds
	$(MAKE) firebird
	$(MAKE) mysql
	$(MAKE) oracle
	$(MAKE) postgresql
	$(MAKE) sqlite
	$(MAKE) sybase

db2: db2.c db2.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ db2.lo $(LIBS)

freetds: freetds.c freetds.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ freetds.lo $(LIBS)

firebird: firebird.c firebird.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ firebird.lo $(LIBS)

mysql: mysql.c mysql.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ mysql.lo $(LIBS)

oracle: oracle.c oracle.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ oracle.lo $(LIBS)

postgresql: postgresql.c postgresql.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ postgresql.lo $(LIBS)

sqlite: sqlite.c sqlite.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ sqlite.lo $(LIBS)

sybase: sybase.c sybase.lo
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ sybase.lo $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o db2 freetds firebird mysql oracle postgresql sqlite sybase
	$(RM) cachefile* sqlnet.log
	$(RMTREE) .libs
