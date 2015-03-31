top_builddir = ../../..

include ../../../config.mk

SRCS= mdbtoolsconnection.cpp
LOBJS= mdbtoolsconnection.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(MDBTOOLSCONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(MDBTOOLSCONNECTIONCPPFLAGS) $(COMPILE) $<

all: sqlrconnection_mdbtools.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_mdbtools.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(MDBTOOLSCONNECTIONLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mdbtools.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_mdbtools.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.a
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_mdbtools.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_mdbtools.*
