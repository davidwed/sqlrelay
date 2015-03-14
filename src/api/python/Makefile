top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(PYTHONLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PYTHONUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = CSQLRelay.cpp
LOBJS = CSQLRelay.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: CSQLRelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo *.o *.so *.pyc
	$(RMTREE) .libs

CSQLRelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(PYTHONDIR)/site-packages/SQLRelay $(RPATH) -avoid-version -module -no-undefined

install: CSQLRelay.la
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(LTINSTALL) CSQLRelay.la $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(RM) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/CSQLRelay.la
	$(RM) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/CSQLRelay.a
	$(INSTALL) -m 644 PySQLRClient.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRDB.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 __init__.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay

uninstall:
	$(RM) $(bindir)/query.py
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
