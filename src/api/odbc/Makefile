top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ODBCDRIVERCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(ODBCDRIVERCPPFLAGS) $(COMPILE) $<

all: libsqlrodbc.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

libsqlrodbc.$(LIBEXT): sqlrodbc.cpp sqlrodbc.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrodbc.$(OBJ) $(LDFLAGS) $(ODBCDRIVERLIBS) $(ODBCDRIVERRPATH) $(LINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrodbc.$(LIBEXT) $(bindir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrodbc.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/libsqlrodbc.a
	$(RM) $(libdir)/libsqlrodbc.$(LIBEXT)

uninstall: $(UNINSTALLLIB)

uninstalldll:
	$(RM) $(bindir)/libsqlrodbc*

uninstalllib:
	$(RM) $(libdir)/libsqlrodbc*
