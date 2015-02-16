top_builddir = ../..

include ../../config.mk

.PHONY: all clean

.SUFFIXES: .lo

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(LIBEFENCE)


all:
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) mysql
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql
endif

mysql.lo: mysql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -I$(top_builddir) $(MYSQLINCLUDES) -c $< -o $@

postgresql.lo: postgresql.cpp
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -I$(top_builddir) $(POSTGRESQLINCLUDES) -c $< -o $@

mysql: mysql.cpp mysql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ mysql.lo $(MYSQLLIBS) $(LIBS)

postgresql: postgresql.cpp postgresql.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ postgresql.lo $(POSTGRESQLLIBS) $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o mysql postgresql
