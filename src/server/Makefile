top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) $(CONFIGFILECPPFLAG) $(CACHEDIRCPPFLAG) $(PSDEFINE) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES)
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-listener$(EXE) \
	sqlr-scaler$(EXE) \
	sqlr-cachemanager$(EXE) \
	sqlr-start$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.lo *.o sqlr-listener$(EXE) sqlr-scaler$(EXE) sqlr-cachemanager$(EXE) sqlr-start$(EXE)
	$(RMTREE) .libs

sqlr-listener$(EXE): sqlrlistener.cpp sqlrlistener.lo sqlr-listener.cpp sqlr-listener.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlrlistener.lo sqlr-listener.lo $(LIBS) $(RPATH)

sqlr-scaler$(EXE): sqlrscaler.cpp sqlrscaler.lo sqlr-scaler.cpp sqlr-scaler.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlrscaler.lo sqlr-scaler.lo $(LIBS) $(RPATH)

sqlr-cachemanager$(EXE): sqlr-cachemanager.cpp sqlr-cachemanager.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-cachemanager.lo $(LIBS) $(RPATH)

sqlr-start$(EXE): sqlr-start.cpp sqlr-start.lo
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -o $@ sqlr-start.lo $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-listener$(EXE) $(bindir)
	$(LTINSTALL) sqlr-scaler$(EXE) $(bindir)
	$(LTINSTALL) sqlr-cachemanager$(EXE) $(bindir)
	$(LTINSTALL) sqlr-start$(EXE) $(bindir)
	$(MKINSTALLDIRS) $(tmpdir)
	chmod 777 $(tmpdir)
	$(MKINSTALLDIRS) $(tmpdir)/sockets
	chmod 777 $(tmpdir)/sockets
	$(MKINSTALLDIRS) $(tmpdir)/pids
	chmod 777 $(tmpdir)/pids
	$(MKINSTALLDIRS) $(tmpdir)/ipc
	chmod 777 $(tmpdir)/ipc

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-listener$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-scaler$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-cachemanager$(EXE)
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-start$(EXE)
