top_builddir = ../..

include ../../config.mk

.PHONY: all clean

.SUFFIXES: .lo

all:
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) mysql
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql
endif

mysql.lo: mysql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(BASECPPFLAGS) -I$(top_builddir) $(MYSQLINCLUDES) -c $< -o $@

postgresql.lo: postgresql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(BASECPPFLAGS) -I$(top_builddir) $(POSTGRESQLINCLUDES) -c $< -o $@

mysql: mysql.cpp mysql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql.lo $(MYSQLLIBS)

postgresql: postgresql.cpp postgresql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql.lo $(POSTGRESQLLIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o mysql postgresql
