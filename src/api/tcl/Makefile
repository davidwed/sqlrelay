include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CXXFLAGS = -DHAVE_CONFIG $(BASECXXFLAGS) $(TCLINCLUDES) -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES) -fPIC
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(TCLLIBS) $(SSLLIB) $(CRYPTOLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = sqlrelayCmd.C
LOBJS = sqlrelayCmd.lo

all: sqlrelay.so

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o	
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
	$(abs_srcdir)/mkinstalldirs /usr/local/lib/sqlrelay
	$(INSTALL) sqlrelay.so /usr/local/lib/sqlrelay
	$(INSTALL) -m 644 pkgIndex.tcl

uninstall:
	$(RMTREE) $(TCLLIBPATH)/sqlrelay
