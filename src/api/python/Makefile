include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(SSLLIB) $(CRYPTOLIB) $(PTHREADLIBS) $(LIBDMALLOC) $(LIBEFENCE)
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
	if ( test "$(STRIP)" != "touch" ); then \
		$(STRIP) -x -R .note -R .comment $@; \
	fi

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
	$(abs_srcdir)/mkinstalldirs $(PYTHONDIR)/site-packages/SQLRelay
	$(LIBTOOL) --mode=install $(INSTALL) CSQLRelay.so $(PYTHONDIR)/site-packages/SQLRelay
	$(LIBTOOL) --mode=install $(INSTALL) -m 644 PySQLRClient.py $(PYTHONDIR)/site-packages/SQLRelay
	$(LIBTOOL) --mode=install $(INSTALL) -m 644 PySQLRDB.py $(PYTHONDIR)/site-packages/SQLRelay
	$(LIBTOOL) --mode=install $(INSTALL) -m 644 __init__.py $(PYTHONDIR)/site-packages/SQLRelay
	$(RM) CSQLRelay.so

uninstall:
	$(RM) $(bindir)/query.py
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
