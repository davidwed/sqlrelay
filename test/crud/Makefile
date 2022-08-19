top_builddir = ../..

include ../../config.mk

.SUFFIXES: .lo .obj .cpp

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPTESTCPPFLAGS) $(COMPILE) $<

all: crud mvccrud.cgi $(MODMVCCRUD)

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

# fedora, ubuntu, older ubuntu, opensuse, old redhat, freebsd, old freebsd,
# openbsd, netbsd, solaris, old solaris, older solaris, os x
DOCROOTS=/var/www/html \
		/var/www/apache2-default \
		/var/www \
		/srv/www/htdocs \
		/home/httpd/html \
		/usr/local/www/apache24/data \
		/usr/local/www/apache22/data \
		/usr/pkg/share/httpd/htdocs \
		/var/www/htdocs \
		/var/apache2/2.4/htdocs \
		/var/apache2/htdocs \
		/var/apache/htdocs \
		/Library/WebServer/Documents

# fedora/old redhat, ubuntu, opensuse, freebsd, old freebsd, openbsd, netbsd,
# solaris, old solaris, older solaris, os x
MODULEDIRS=/etc/httpd/modules \
		/usr/lib/apache2/modules \
		/usr/lib64/apache2 \
		/usr/local/libexec/apache24 \
		/usr/local/libexec/apache22 \
		/usr/pkg/lib/httpd \
		/usr/local/lib/apache2 \
		/usr/apache2/2.4/libexec \
		/usr/apache2/libexec \
		/usr/apache/libexec \
		/usr/libexec/httpd

installcgi:
	for path in $(DOCROOTS); \
	do \
		if ( test -d "$$path" ); \
		then \
			$(LTINSTALL) $(CP) mvccrud.cgi $$path; \
			break; \
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
			break; \
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
