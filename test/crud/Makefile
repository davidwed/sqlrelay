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
	$(LTLINK) $(LINK) $(OUT)$@ mvccrud-apache.$(OBJ) $(LDFLAGS) $(CPPTESTLIBS) -lrudiments-apache $(APACHELINKFLAGS)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii crud$(EXE) mvccrud.cgi
	$(RMTREE) .libs

install: installcgi installmod

DOCROOTS=/var/www/html \
		/home/httpd/html

MODULEDIRS=/etc/httpd/modules \
		/usr/libexec/httpd/modules

installcgi:
	for path in $(DOCROOTS); \
	do \
		if ( test -d "$$path" ); \
		then \
			$(LTINSTALL) $(CP) mvccrud.cgi $$path; \
		fi; \
	done

installmod:
	for path in $(MODULEDIRS); \
	do \
		if ( test -d "$$path" ); \
		then \
			$(LTINSTALL) $(CP) mod_mvccrud.$(LIBEXT) $$path; \
			$(RM) $$path/mod_mvccrud.a; \
			$(RM) $$path/mod_mvccrud.la; \
		fi; \
	done

uninstall:
	for path in $(DOCROOTS); \
	do \
		$(RM) $$path/mvccrud.cgi; \
	done
	for path in $(MODULEDIRS); \
	do \
		$(RM) $$path/mod_mvccrud.*; \
	done
