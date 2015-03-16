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

db2: db2.c db2.$(OBJ)
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ db2.$(OBJ) $(LIBS)

freetds: freetds.c freetds.$(OBJ)
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ freetds.$(OBJ) $(LIBS)

firebird: firebird.c firebird.$(OBJ)
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ firebird.$(OBJ) $(LIBS)

mysql: mysql.c mysql.$(OBJ)
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ mysql.$(OBJ) $(LIBS)

oracle: oracle.c oracle.$(OBJ)
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ oracle.$(OBJ) $(LIBS)

postgresql: postgresql.c postgresql.$(OBJ)
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ postgresql.$(OBJ) $(LIBS)

sqlite: sqlite.c sqlite.$(OBJ)
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ sqlite.$(OBJ) $(LIBS)

sybase: sybase.c sybase.$(OBJ)
	$(LIBTOOLGCC) --mode=link $(CC) $(LDFLAGS) -o $@ sybase.$(OBJ) $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o db2 freetds firebird mysql oracle postgresql sqlite sybase
	$(RM) cachefile* sqlnet.log
	$(RMTREE) .libs
