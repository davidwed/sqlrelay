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

mysql.lo: mysql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(BASECPPFLAGS) -I$(top_builddir) $(MYSQLINCLUDES) -c $< -o $@

postgresql.lo: postgresql.C
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(BASECPPFLAGS) -I$(top_builddir) $(POSTGRESQLINCLUDES) -c $< -o $@

mysql: mysql.C mysql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql.lo $(MYSQLLIBS)

postgresql: postgresql.C postgresql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql.lo $(POSTGRESQLLIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) mysql
	$(LIBTOOL) --mode=clean $(RM) postgresql
