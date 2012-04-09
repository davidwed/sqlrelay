top_builddir = ../..

include ../../config.mk

.PHONY: all clean

CPPFLAGS = $(BASECPPFLAGS) -I$(includedir) $(RUDIMENTSINCLUDES)
LIBS = -L$(libdir) -lsqlrclient $(RUDIMENTSLIBS) $(LIBEFENCE)

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: cursortest forktest query querytest random socketeater threadtest timeouttest

cursortest: cursortest.cpp cursortest.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ cursortest.lo $(LIBS)

forktest: forktest.cpp forktest.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ forktest.lo $(LIBS)

query: query.cpp query.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ query.lo $(LIBS)

querytest: querytest.cpp querytest.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ querytest.lo $(LIBS)

random: random.cpp random.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ random.lo $(LIBS)

socketeater: socketeater.cpp socketeater.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ socketeater.lo $(LIBS)

threadtest: threadtest.cpp threadtest.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ threadtest.lo $(LIBS)

timeouttest: timeouttest.cpp timeouttest.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ timeouttest.lo $(LIBS)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(LIBTOOL) --mode=clean $(RM) cursortest
	$(LIBTOOL) --mode=clean $(RM) forktest
	$(LIBTOOL) --mode=clean $(RM) query
	$(LIBTOOL) --mode=clean $(RM) querytest
	$(LIBTOOL) --mode=clean $(RM) random
	$(LIBTOOL) --mode=clean $(RM) socketeater
	$(LIBTOOL) --mode=clean $(RM) threadtest
	$(LIBTOOL) --mode=clean $(RM) timeouttest
	$(RMTREE) .libs
