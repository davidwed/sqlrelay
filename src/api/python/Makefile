include ../../../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) -I$(abs_srcdir)/src/common -I$(abs_srcdir)/src/api/c++/include $(RUDIMENTSINCLUDES)
LIBS = -L$(abs_srcdir)/src/api/c++/src -lsqlrclient $(RUDIMENTSLIBS) $(STDCPPLIB) $(PYTHONLIBS) $(SSLLIB) $(CRYPTOLIB) $(PTHREADLIBS) $(LIBDMALLOC) $(LIBEFENCE) 

SRCS = CSQLRelay.C
LOBJS = CSQLRelay.lo python.lo

.SUFFIXES: .lo

.C.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: CSQLRelay.so

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o	
	$(LIBTOOL) --mode=clean $(RM) CSQLRelay.so

rebuild: clean all

python.lo:
	cp $(PYTHONOBJ) $@

CSQLRelay.so: libCSQLRelay.la
	for i in `find .libs -name "libCSQLRelay*so*"`; \
	do \
		cp $$i CSQLRelay.so; \
	done
	$(LIBTOOL) --mode=clean $(RM) *.la

libCSQLRelay.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO)


install:
	$(abs_srcdir)/mkinstalldirs $(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) CSQLRelay.so $(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRClient.py $(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRDB.py $(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 __init__.py $(PYTHONDIR)/site-packages/SQLRelay

uninstall:
	$(RM) $(bindir)/query.py
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
