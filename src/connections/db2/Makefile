top_builddir = ../../..

include ../../../config.mk

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/server/include $(RUDIMENTSINCLUDES) $(DB2INCLUDES)
LIBS = $(DB2LIBS) -L$(top_builddir)/src/server/src -lsqlrserver -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(MATHLIB) $(EXTRALIBS)
ifneq ($(strip $(DB2LIBSPATH)),)
ifeq ($(DB2USERPATH),yes)
	RPATH = -R $(DB2LIBSPATH)
endif
endif

SRCS= db2connection.cpp
LOBJS= db2connection.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE) $<

all: sqlrconnection_db2.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

sqlrconnection_db2.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(LIBS) $(RPATH) $(MODLINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_db2.$(LIBEXT) $(libexecdir)

installlib:
ifeq ($(SQLRELAY_ENABLE_SHARED),yes)
	$(MKINSTALLDIRS) $(libexecdir)
	$(LTINSTALL) $(CP) sqlrconnection_db2.$(LIBEXT) $(libexecdir)
	$(RM) $(libexecdir)/sqlrconnection_db2.a
	$(RM) $(libexecdir)/sqlrconnection_db2.$(LIBEXT)
	$(MODULERENAME) $(libexecdir)/sqlrconnection_db2.so so $(MODULESUFFIX)
endif

uninstall:
	$(RM) $(libexecdir)/sqlrconnection_db2.*
