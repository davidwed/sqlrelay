include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CXXFLAGS = -DHAVE_CONFIG $(BASECXXFLAGS) $(TCLINCLUDES) -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES) -fPIC
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(TCLLIBS) $(SSLLIB) $(CRYPTOLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = sqlrelayCmd.C
LOBJS = sqlrelayCmd.lo

all:
	cp tclincludes1.h tclincludes.h; \
	$(MAKE) sqlrelay.so 2> errors1; \
	if ( test ! -r sqlrelay.so ); then \
		cp tclincludes2.h tclincludes.h; \
		$(MAKE) sqlrelay.so 2> errors2; \
	fi
	if ( test ! -r sqlrelay.so ); then \
		if ( test -s errors1 ); then \
			echo "first try errors : "; cat errors1; echo; \
		fi; \
		if ( test -s errors2 ); then \
			echo "second try errors : "; cat errors2; echo; \
		fi; \
	fi
	$(RM) errors1
	$(RM) errors2

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o	
	$(LIBTOOL) --mode=clean $(RM) tclincludes.h
	$(LIBTOOL) --mode=clean $(RM) errors1
	$(LIBTOOL) --mode=clean $(RM) errors2
	$(LIBTOOL) --mode=clean $(RM) sqlrelay.so

rebuild: clean all

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

sqlrelay.so: libsqlrelay.la
	for i in `find .libs -name "libsqlrelay*so*"`; \
	do \
		cp -L $$i sqlrelay.so; \
	done
	$(LIBTOOL) --mode=clean $(RM) *.la

libsqlrelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)

install:
	$(abs_srcdir)/mkinstalldirs $(TCLLIBPATH)/sqlrelay
	$(INSTALL) sqlrelay.so $(TCLLIBPATH)/sqlrelay
	$(INSTALL) -m 644 pkgIndex.tcl $(TCLLIBPATH)/sqlrelay

uninstall:
	$(RMTREE) $(TCLLIBPATH)/sqlrelay
