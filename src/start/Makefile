include ../../config.mk

top_srcdir = ../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common -I../util $(RUDIMENTSINCLUDES) $(PSDEFINE)
LIBS = $(RUDIMENTSLIBS) -L../util -lsqlrutil $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = main.C
OBJS = main.o

all: sqlr-start

clean:
	$(RM) *.o
	$(RM) sqlr-start

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

sqlr-start: $(SRCS) $(OBJS)
	$(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	$(INSTALL) sqlr-start $(bindir)

uninstall:
	$(RM) $(bindir)/sqlr-start
