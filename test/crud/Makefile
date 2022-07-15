top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(COMPILE) $<

all: crud mvccrud.cgi mod_mvccrud.$(LIBEXT)

crud: crud.cpp crud.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) crud.$(OBJ) $(CPPTESTLIBS)

mvccrud.cgi: mvccrud.cpp mvccrud.$(OBJ)
	$(LTLINK) $(LINK) $(LDFLAGS) $(OUT)$@$(EXE) mvccrud.$(OBJ) $(CPPTESTLIBS)

mvccrud-apache.$(OBJ): mvccrud.cpp
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) `rudiments-config --apache-cflags` -DAPACHE $(COMPILE) $< $(OUT)$@

mod_mvccrud.$(LIBEXT): mvccrud.cpp mvccrud-apache.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ mvccrud-apache.$(OBJ) $(LDFLAGS) $(CPPTESTLIBS) -lrudiments-apache $(MODLINKFLAGS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii crud$(EXE) mvccrud.cgi
	$(RMTREE) .libs

install:
	$(LTINSTALL) $(CP) mvccrud.cgi /var/www/html
	$(LTINSTALL) $(CP) mod_mvccrud.$(LIBEXT) /etc/httpd/modules
	$(RM) /etc/httpd/modules/mod_mvccrud.a
	$(RM) /etc/httpd/modules/mod_mvccrud.la
	systemctl restart httpd

uninstall:
	$(RM) /var/www/html/mvccrud.cgi
	$(RM) /etc/httpd/modules/mod_mvccrud.*
