include ../../config.mk

LIBS=-L/usr/local/firstworks/lib -lsqlrclient -lrudiments -lm -lefence

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) -f db2
	$(LIBTOOL) --mode=clean $(RM) -f freetds
	$(LIBTOOL) --mode=clean $(RM) -f interbase
	$(LIBTOOL) --mode=clean $(RM) -f lago
	$(LIBTOOL) --mode=clean $(RM) -f msql
	$(LIBTOOL) --mode=clean $(RM) -f mysql
	$(LIBTOOL) --mode=clean $(RM) -f oracle7
	$(LIBTOOL) --mode=clean $(RM) -f oracle8
	$(LIBTOOL) --mode=clean $(RM) -f oracle8i
	$(LIBTOOL) --mode=clean $(RM) -f postgresql6
	$(LIBTOOL) --mode=clean $(RM) -f postgresql7
	$(LIBTOOL) --mode=clean $(RM) -f sqlite
	$(LIBTOOL) --mode=clean $(RM) -f sybase

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
ifneq ($(strip $(LAGOLIBS)),)
	$(MAKE) lago
endif
ifneq ($(strip $(MSQLLIBS)),)
	$(MAKE) msql
endif
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) mysql
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) oracle7
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
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ db2.lo $(LIBS)

freetds: freetds.c freetds.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ freetds.lo $(LIBS)

interbase: interbase.c interbase.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ interbase.lo $(LIBS)

lago: lago.c lago.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ lago.lo $(LIBS)

msql: msql.c msql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ msql.lo $(LIBS)

mysql: mysql.c mysql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql.lo $(LIBS)

oracle7: oracle7.c oracle7.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle7.lo $(LIBS)

oracle8: oracle8.c oracle8.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8.lo $(LIBS)

oracle8i: oracle8i.c oracle8i.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ oracle8i.lo $(LIBS)

postgresql6: postgresql6.c postgresql6.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql6.lo $(LIBS)

postgresql7: postgresql7.c postgresql7.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql7.lo $(LIBS)

sqlite: sqlite.c sqlite.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlite.lo $(LIBS)

sybase: sybase.c sybase.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sybase.lo $(LIBS)


rebuild: clean all
