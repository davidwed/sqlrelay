top_builddir = ../../..

include ../../../config.mk

SRCS= mysqlconnection.cpp
LOBJS= mysqlconnection.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MYSQLCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(MYSQLCONNECTIONCPPFLAGS) $(COMPILE) $<

all: sqlrconnection_mysql.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_mysql.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(MYSQLCONNECTIONLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mysql.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mysql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_mysql.a
	$(RM) $(libexecdir)/sqlrconnection_mysql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_mysql.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_mysql.*
