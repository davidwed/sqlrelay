top_builddir = ../..

include ../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = -L$(libdir) -lsqlrclient $(RUDIMENTSLIBS) $(LIBEFENCE)

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all:
	$(MAKE) db2
	$(MAKE) db27
	$(MAKE) db26
	$(MAKE) freetds
	$(MAKE) firebird
	$(MAKE) mysql
	$(MAKE) oracle
	$(MAKE) oracle8
	$(MAKE) oracle7
	$(MAKE) postgresql
	$(MAKE) sqlite
	$(MAKE) sybase
	$(MAKE) router

db2: db2.cpp db2.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db2.$(OBJ) $(LIBS)

db27: db27.cpp db27.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db27.$(OBJ) $(LIBS)

db26: db26.cpp db26.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db26.$(OBJ) $(LIBS)

freetds: freetds.cpp freetds.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ freetds.$(OBJ) $(LIBS)

firebird: firebird.cpp firebird.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ firebird.$(OBJ) $(LIBS)

mysql: mysql.cpp mysql.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql.$(OBJ) $(LIBS)

oracle: oracle.cpp oracle.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle.$(OBJ) $(LIBS)

oracle8: oracle8.cpp oracle8.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8.$(OBJ) $(LIBS)
oracle7: oracle7.cpp oracle7.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle7.$(OBJ) $(LIBS)

postgresql: postgresql.cpp postgresql.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql.$(OBJ) $(LIBS)

oracleclobinsert: oracleclobinsert.cpp oracleclobinsert.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracleclobinsert.$(OBJ) $(LIBS)

oracleclobfetch: oracleclobfetch.cpp oracleclobfetch.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracleclobfetch.$(OBJ) $(LIBS)

sqlite: sqlite.cpp sqlite.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlite.$(OBJ) $(LIBS)

sybase: sybase.cpp sybase.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sybase.$(OBJ) $(LIBS)

router: router.cpp router.$(OBJ)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ router.$(OBJ) $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o db2 db27 db26 freetds firebird mysql oracleclobfetch oracleclobinsert oracle oracle8 oracle7 postgresql sqlite sybase router
	$(RM) cachefile* sqlnet.log
	$(RMTREE) .libs
