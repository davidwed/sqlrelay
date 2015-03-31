top_builddir = ../../..

include ../../../config.mk

SRCS= oracle8connection.cpp
LOBJS= oracle8connection.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ORACLECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(ORACLECONNECTIONCPPFLAGS) $(COMPILE) $<

all: sqlrconnection_oracle8.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_oracle8.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(ORACLECONNECTIONLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_oracle8.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_oracle8.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_oracle8.a
	$(RM) $(libexecdir)/sqlrconnection_oracle8.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_oracle8.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_oracle8.*
