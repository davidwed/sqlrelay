include ../../config.mk

top_srcdir = ../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common -I../util $(RUDIMENTSINCLUDES)
LIBS = $(RUDIMENTSLIBS) -L../util -lsqlrutil $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = scaler.C main.C
OBJS = scaler.o main.o

all: sqlr-scaler

clean:
	$(RM) *.o
	$(RM) sqlr-scaler

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

sqlr-scaler: $(SRCS) $(OBJS)
	$(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	$(INSTALL) sqlr-scaler $(bindir)

uninstall:
	$(RM) $(bindir)/sqlr-scaler
