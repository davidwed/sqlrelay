top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(ODBCDRIVERCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(ODBCDRIVERCPPFLAGS) $(COMPILE) $<

all: lib$(SQLR)odbc.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

lib$(SQLR)odbc.$(LIBEXT): sqlrodbc.cpp sqlrodbc.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrodbc.$(OBJ) $(LDFLAGS) $(ODBCDRIVERLIBS) $(ODBCDRIVERRPATH) $(CLTLINKFLAGS)

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

sqlrodbc.def:
	echo "LIBRARY LIBSQLRODBC" > $@
	echo "EXPORTS" >> $@
	grep " SQL_API " *.cpp | cut -d' ' -f3 | cut -d'(' -f1 >> $@
	grep " INSTAPI " *.cpp | cut -d' ' -f3 | cut -d'(' -f1 >> $@
