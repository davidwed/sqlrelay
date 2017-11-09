top_builddir = ../../..

include ../../../config.mk

SRCS = sqlrconnection.cpp sqlrcursor.cpp
LOBJS = sqlrconnection.$(OBJ) sqlrcursor.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(CPPCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(CPPCPPFLAGS) $(COMPILE) $<

all: lib$(SQLR)client.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

lib$(SQLR)client.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(LDFLAGS) $(CLTLINKFLAGS) $(CPPLIBS)

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(cachedir)
	$(CHMOD) 777 $(cachedir)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(CP) sqlrelay/sqlrclient.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrclient.h
	$(CP) sqlrelay/private/sqlrclientincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrclientincludes.h
	$(CP) sqlrelay/private/sqlrconnection.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrconnection.h
	$(CP) sqlrelay/private/sqlrcursor.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrcursor.h

installdll::
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) lib$(SQLR)client.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) lib$(SQLR)client.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) lib$(SQLR)client.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RMTREE) $(cachedir) \
		$(localstatedir)/$(SQLRELAY)/cache \
		$(localstatedir)/sqlrelay/cache
	$(RM) $(includedir)/sqlrclient.h \
		$(includedir)/private/sqlrclientincludes.h \
		$(includedir)/private/sqlrconnection.h \
		$(includedir)/private/sqlrcursor.h \
		$(includedir)/private/bindvar.h \
		$(includedir)/private/column.h \
		$(includedir)/private/field.h \
		$(includedir)/private/row.h \
		$(includedir)/private/listformat.h \
		$(includedir)/private/sqlrdefines.h \
		$(includedir)/private/sqlrincludes.h \
		$(includedir)/private/sqlrclientdll.h \
		$(includedir)/private/dll.h
	$(RMTREE) $(includedir)/sqlrelay

uninstalldll:
	$(RM) $(bindir)/lib$(SQLR)client.$(LIBEXT) \
		$(libdir)/lib$(SQLR)client.lib \
		$(bindir)/libsqlrclient.$(LIBEXT) \
		$(libdir)/libsqlrclient.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/lib$(SQLR)client.$(LIBEXT) \
				$(libdir)/libsqlrclient.$(LIBEXT)
