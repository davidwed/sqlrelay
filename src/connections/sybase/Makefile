top_builddir = ../../..

include ../../../config.mk

SRCS= sybaseconnection.cpp
LOBJS= sybaseconnection.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(SYBASECONNECTIONCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(SYBASECONNECTIONCPPFLAGS) $(COMPILE) $<

all: sqlrconnection_sybase.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_sybase.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(SYBASECONNECTIONLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sybase.$(LIBEXT) $(libexecdir)

installlib: $(INSTALLSHAREDLIB)

installsharedlib:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_sybase.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_sybase.a
	$(RM) $(libexecdir)/sqlrconnection_sybase.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_sybase.so so $(MODULESUFFIX)

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_sybase.*
