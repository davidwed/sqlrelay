include ../../../config.mk

top_srcdir = ../../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = -DHAVE_CONFIG $(BASECPPFLAGS) $(PYTHONINCLUDES) -I../c++/include $(RUDIMENTSINCLUDES)
LIBS = -L../c++/src/.libs -lsqlrclient $(RUDIMENTSLIBS) $(PYTHONLIBS) $(PYTHONOBJ) $(LIBSTDCPP) $(SSLLIB) $(CRYPTOLIB) $(LIBDMALLOC) $(LIBEFENCE)

SRCS = CSQLRelay.C
OBJS = CSQLRelay.o

all: CSQLRelay.so

clean:
	$(RM) *.o *~ *.so

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

CSQLRelay.so: $(SRCS) $(OBJS)
	$(DYNAMICAR) $@ $(OBJS) $(LIBS)

install:
	$(top_srcdir)/mkinstalldirs $(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) CSQLRelay.so $(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRClient.py $(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 PySQLRDB.py $(PYTHONDIR)/site-packages/SQLRelay
	$(INSTALL) -m 644 __init__.py $(PYTHONDIR)/site-packages/SQLRelay

uninstall:
	$(RM) $(bindir)/query.py
	$(RMTREE) $(PYTHONDIR)/site-packages/SQLRelay
