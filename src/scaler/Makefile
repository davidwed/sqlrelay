include ../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) $(TMPDIRCPPFLAG) $(CONFIGFILECPPFLAG) -I./ -I$(abs_srcdir)/ -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/util $(RUDIMENTSINCLUDES)
LIBS = $(abs_srcdir)/src/util/libsqlrutil.la $(RUDIMENTSLIBS) $(PTHREADLIBS) $(EXTRALIBS) $(SOCKETLIB) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(CLIENTUSERPATH),yes)
	RPATH = -rpath $(libdir)
endif
endif

SRCS = scaler.C main.C
LOBJS = scaler.lo main.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-scaler

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-scaler
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o

rebuild: clean all

sqlr-scaler: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(STATICLINK) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(abs_srcdir)/mkinstalldirs $(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) $(INSTALLSTRIP) sqlr-scaler $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-scaler
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-scaler-debug
