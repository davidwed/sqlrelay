include ../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util $(RUDIMENTSINCLUDES) $(PSDEFINE)
LIBS = $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
DEBUGLIBS = $(abs_srcdir)/src/util/libsqlrutil_p.la $(RUDIMENTSDEBUGLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = main.C
NLOBJS = main.n.lo
PLOBJS = main.p.lo

.SUFFIXES: .n.lo .p.lo

.C.n.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.C.p.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

#all: sqlr-start sqlr-start-debug
all: sqlr-start

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-start
	$(LIBTOOL) --mode=clean $(RM) sqlr-start-debug
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-start: $(SRCS) $(NLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(NLOBJS) $(LIBS) -rpath $(libdir)

sqlr-start-debug: $(SRCS) $(PLOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(PLOBJS) $(DEBUGLIBS) -rpath $(libdir)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) -s sqlr-start $(bindir)
	#$(LIBTOOL) --mode=install $(INSTALL) sqlr-start-debug $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-start
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-start-debug
