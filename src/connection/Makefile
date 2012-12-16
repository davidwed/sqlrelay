top_builddir = ../..

include ../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(WERROR) $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util $(RUDIMENTSINCLUDES) -DLIBEXECDIR=\"$(libexecdir)\"
LIBS = -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS)

SRCS = sqlrcontroller.cpp \
	sqlrconnection.cpp \
	sqlrcursor.cpp \
	sqlparser.cpp \
	sqlwriter.cpp \
	sqltranslations.cpp \
	sqltranslation.cpp \
	sqltriggers.cpp \
	sqltrigger.cpp \
	sqlrloggers.cpp \
	sqlrlogger.cpp \
	sqlrqueries.cpp \
	sqlrquery.cpp

LOBJS = sqlrcontroller.lo \
	sqlrconnection.lo \
	sqlrcursor.lo \
	sqlparser.lo \
	sqlwriter.lo \
	sqltranslations.lo \
	sqltranslation.lo \
	sqltriggers.lo \
	sqltrigger.lo \
	sqlrloggers.lo \
	sqlrlogger.lo \
	sqlrqueries.lo \
	sqlrquery.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: libsqlrconnection.la \
	sqlr-connection$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) *.la *.lo */*.lo *.o */*.o sqlr-connection$(EXE)
	$(RMTREE) */.libs .libs

libsqlrconnection.la: $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(LOBJS) $(LDFLAGS) $(LIBS) -rpath $(libdir) -release $(SQLR_VERSION) -version-info $(SONAME_VERSION_INFO) -no-undefined

sqlr-connection$(EXE): sqlr-connection.cpp sqlr-connection.lo 
	$(LIBTOOL) --mode=link $(CXX) -o $@ sqlr-connection.lo $(LDFLAGS) libsqlrconnection.la $(LIBS) $(RPATH)
	
	
install:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) libsqlrconnection.la $(libdir)
	$(LIBTOOL) --mode=finish $(libdir)
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection$(EXE) $(bindir)
	$(MKINSTALLDIRS) $(tmpdir)
	chmod 777 $(tmpdir)
	$(MKINSTALLDIRS) $(tmpdir)/sockets
	chmod 777 $(tmpdir)/sockets
	$(MKINSTALLDIRS) $(tmpdir)/pids
	chmod 777 $(tmpdir)/pids
	$(MKINSTALLDIRS) $(tmpdir)/ipc
	chmod 777 $(tmpdir)/ipc
	$(MKINSTALLDIRS) $(logdir)
	chmod 777 $(logdir)
	$(MKINSTALLDIRS) $(debugdir)
	chmod 777 $(debugdir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(libdir)/libsqlrconnection.la
	$(RMTREE) $(tmpdir)
	$(RMTREE) $(prefix)/lib/sqlrelay/tmp
	$(RMTREE) $(logdir)
	$(RMTREE) $(debugdir)
	$(RM) $(libdir)/libsqlrelay_*
	$(RM) $(libexecdir)/libsqlrelay_*
	$(RM) $(libdir)/libsqlrconnection_p.*
	$(RM) $(bindir)/sqlr-connection
