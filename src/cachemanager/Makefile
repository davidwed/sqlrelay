include ../../config.mk

top_srcdir = ../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -DCACHE_DIR=\"$(cachedir)\" -I./ -I../../ -I../common $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) $(EXTRALIBS) $(SOCKETLIB)

SRCS = cachemanager.C main.C
OBJS = cachemanager.o main.o

all: sqlr-cachemanager

clean:
	$(RM) *.o
	$(RM) sqlr-cachemanager

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

sqlr-cachemanager: $(SRCS) $(OBJS)
	$(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	$(INSTALL) sqlr-cachemanager $(bindir)

uninstall:
	$(RM) $(bindir)/sqlr-cachemanager
