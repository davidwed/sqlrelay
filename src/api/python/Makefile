top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES) $(SSLINCLUDES)
LIBS = -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(SSLLIBS) $(PTHREADLIBS) $(LIBDMALLOC) $(LIBEFENCE)
RPATH =
ifneq ($(strip $(libdir)),)
ifeq ($(PYTHONUSERPATH),yes)
	RPATH = -R $(libdir)
endif
endif

SRCS = CSQLRelay.C
LOBJS = CSQLRelay.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	$(STRIPCOMMENTNOTE) $@

all: libCSQLRelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o	
	$(LIBTOOL) --mode=clean $(RM) *.so

rebuild: clean all

libCSQLRelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LIBS) -rpath $(PYTHONDIR)/site-packages/SQLRelay $(RPATH) -module

CSQLRelay.so:
	if ( test -r .libs/libCSQLRelay.0.0.0 ); \
	then \
		cp .libs/libCSQLRelay.0.0.0 CSQLRelay.so; \
	fi
	if ( test -r .libs/libCSQLRelay.0.0 ); \
	then \
		cp .libs/libCSQLRelay.0.0 CSQLRelay.so; \
	fi
	if ( test -r .libs/libCSQLRelay.0 ); \
	then \
		cp .libs/libCSQLRelay.0 CSQLRelay.so; \
	fi
	if ( test -r .libs/libCSQLRelay ); \
	then \
		cp .libs/libCSQLRelay CSQLRelay.so; \
	fi
	if ( test -r .libs/libCSQLRelay.so.0.0.0 ); \
	then \
		cp .libs/libCSQLRelay.so.0.0.0 CSQLRelay.so; \
	fi
	if ( test -r .libs/libCSQLRelay.so.0.0 ); \
	then \
		cp .libs/libCSQLRelay.so.0.0 CSQLRelay.so; \
	fi
	if ( test -r .libs/libCSQLRelay.so.0 ); \
	then \
		cp .libs/libCSQLRelay.so.0 CSQLRelay.so; \
	fi
	if ( test -r .libs/libCSQLRelay.so ); \
	then \
		cp .libs/libCSQLRelay.so CSQLRelay.so; \
	fi

install: CSQLRelay.so
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) $(INSTALLSTRIP) CSQLRelay.so $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRClient.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRDB.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 __init__.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(RM) CSQLRelay.so

uninstall:
	$(RM) $(bindir)/query.py
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
