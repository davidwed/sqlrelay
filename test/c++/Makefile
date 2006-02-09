top_builddir = ../..

include ../../config.mk

.PHONY: all clean rebuild

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
ifneq ($(strip $(INTERBASELIBS)),)
	$(MAKE) interbase
endif
ifneq ($(strip $(MSQLLIBS)),)
	$(MAKE) msql
endif
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) mysql3
	$(MAKE) mysql412
	$(MAKE) mysql5
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) oracle7
	$(MAKE) oracle8
	$(MAKE) oracle8i
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql6
	$(MAKE) postgresql7
	$(MAKE) postgresql8
endif
ifneq ($(strip $(SQLITELIBS)),)
	$(MAKE) sqlite
endif
ifneq ($(strip $(SYBASELIBS)),)
	$(MAKE) sybase
endif

db2: db2.C db2.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db2.lo $(LIBS)

freetds: freetds.C freetds.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ freetds.lo $(LIBS)

interbase: interbase.C interbase.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ interbase.lo $(LIBS)

msql: msql.C msql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ msql.lo $(LIBS)

mysql3: mysql3.C mysql3.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql3.lo $(LIBS)

mysql412: mysql412.C mysql412.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql412.lo $(LIBS)

mysql5: mysql5.C mysql5.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql5.lo $(LIBS)

oracle7: oracle7.C oracle7.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle7.lo $(LIBS)

oracle8: oracle8.C oracle8.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8.lo $(LIBS)

oracle8i: oracle8i.C oracle8i.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8i.lo $(LIBS)

oracle8iclobinsert: oracle8iclobinsert.C oracle8iclobinsert.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8iclobinsert.lo $(LIBS)

oracle8iclobfetch: oracle8iclobfetch.C oracle8iclobfetch.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8iclobfetch.lo $(LIBS)

postgresql6: postgresql6.C postgresql6.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql6.lo $(LIBS)

postgresql7: postgresql7.C postgresql7.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql7.lo $(LIBS)

postgresql8: postgresql8.C postgresql8.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql8.lo $(LIBS)

sqlite: sqlite.C sqlite.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlite.lo $(LIBS)

sybase: sybase.C sybase.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sybase.lo $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) db2
	$(LIBTOOL) --mode=clean $(RM) freetds
	$(LIBTOOL) --mode=clean $(RM) interbase
	$(LIBTOOL) --mode=clean $(RM) msql
	$(LIBTOOL) --mode=clean $(RM) mysql3
	$(LIBTOOL) --mode=clean $(RM) mysql412
	$(LIBTOOL) --mode=clean $(RM) mysql5
	$(LIBTOOL) --mode=clean $(RM) oracle7
	$(LIBTOOL) --mode=clean $(RM) oracle8
	$(LIBTOOL) --mode=clean $(RM) oracle8iclobfetch
	$(LIBTOOL) --mode=clean $(RM) oracle8iclobinsert
	$(LIBTOOL) --mode=clean $(RM) oracle8i
	$(LIBTOOL) --mode=clean $(RM) postgresql6
	$(LIBTOOL) --mode=clean $(RM) postgresql7
	$(LIBTOOL) --mode=clean $(RM) postgresql8
	$(LIBTOOL) --mode=clean $(RM) sqlite
	$(LIBTOOL) --mode=clean $(RM) sybase
	$(RM) cachefile*
	$(RM) sqlnet.log
	$(RMTREE) .libs

rebuild: clean all
