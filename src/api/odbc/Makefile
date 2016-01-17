top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ODBCDRIVERCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(ODBCDRIVERCPPFLAGS) $(COMPILE) $<

all: lib$(SQLR)odbc.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest
	$(RMTREE) .libs

lib$(SQLR)odbc.$(LIBEXT): sqlrodbc.cpp sqlrodbc.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrodbc.$(OBJ) $(LDFLAGS) $(ODBCDRIVERLIBS) $(ODBCDRIVERRPATH) $(LINKFLAGS)

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) lib$(SQLR)odbc.$(LIBEXT) $(bindir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) lib$(SQLR)odbc.$(LIBEXT) $(libdir)
	$(RM) $(libdir)/lib$(SQLR)odbc.a
	$(RM) $(libdir)/lib$(SQLR)odbc.$(LIBEXT)

uninstall: $(UNINSTALLLIB)

uninstalldll:
	$(RM) $(bindir)/lib$(SQLR)odbc* \
		$(bindir)/libsqlrodbc*

uninstalllib:
	$(RM) $(libdir)/lib$(SQLR)odbc* \
		$(libdir)/libsqlrodbc*
