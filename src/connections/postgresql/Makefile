top_builddir = ../../..

include ../../../config.mk

SRCS= postgresqlconnection.cpp
LOBJS= postgresqlconnection.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(POSTGRESQLCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(POSTGRESQLCONNECTIONCPPFLAGS) $(COMPILE) $<

all: sqlrconnection_postgresql.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_postgresql.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(POSTGRESQLCONNECTIONLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_postgresql.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_postgresql.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_postgresql.a
	$(RM) $(libexecdir)/sqlrconnection_postgresql.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_postgresql.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_postgresql.*
