top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean rebuild install uninstall

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) -I$(top_builddir)/src/common -I$(top_builddir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = $(PYTHONLIB) -L$(top_builddir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(LIBDMALLOC) $(LIBEFENCE)
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

all: libCSQLRelay.la

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o	
	$(LIBTOOL) --mode=clean $(RM) *.so
	$(RMTREE) .libs

rebuild: clean all

libCSQLRelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(PYTHONDIR)/site-packages/SQLRelay $(RPATH) -module -no-undefined

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

CSQLRelay.dll:
	if ( test -r .libs/cygCSQLRelay-0.dll ); \
	then \
		cp .libs/cygCSQLRelay-0.dll CSQLRelay.dll; \
	fi

ifeq ($(CYGWIN),)
install: CSQLRelay.so
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) CSQLRelay.so $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRClient.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
ifeq ($(PYTHON_HAVE_WEAKREF),yes)
	$(INSTALL) -m 644 PySQLRDB.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
else
	$(INSTALL) -m 644 PySQLRDB-noweakref.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay/PySQLRDB.py
endif
	$(INSTALL) -m 644 __init__.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(RM) CSQLRelay.so
else
install: CSQLRelay.dll
	$(MKINSTALLDIRS) $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) CSQLRelay.dll $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRClient.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRDB.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 __init__.py $(DESTDIR)$(PYTHONDIR)/site-packages/SQLRelay
	$(RM) CSQLRelay.dll
endif

uninstall:
	$(RM) $(bindir)/query.py
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
