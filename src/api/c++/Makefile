top_builddir = ../../..

include ../../../config.mk

SRCS = sqlrconnection.cpp \
	sqlrcursor.cpp \
	sqlrresultsetdomnode.cpp \
	sqlrimportxml.cpp \
	sqlrimportcsv.cpp
LOBJS = sqlrconnection.$(OBJ) \
	sqlrcursor.$(OBJ) \
	sqlrresultsetdomnode.$(OBJ) \
	sqlrimportxml.$(OBJ) \
	sqlrimportcsv.$(OBJ)

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
	$(CP) sqlrelay/sqlrresultsetdomnode.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrresultsetdomnode.h
	$(CP) sqlrelay/sqlrimportxml.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrimportxml.h
	$(CP) sqlrelay/sqlrimportcsv.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrimportcsv.h
	$(CP) sqlrelay/private/sqlrclientincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrclientincludes.h
	$(CP) sqlrelay/private/sqlrconnection.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrconnection.h
	$(CP) sqlrelay/private/sqlrcursor.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrcursor.h
	$(CP) sqlrelay/private/sqlrresultsetdomnodeincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsetdomnodeincludes.h
	$(CP) sqlrelay/private/sqlrresultsetdomnode.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrresultsetdomnode.h
	$(CP) sqlrelay/private/sqlrimportxmlincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportxmlincludes.h
	$(CP) sqlrelay/private/sqlrimportxml.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportxml.h
	$(CP) sqlrelay/private/sqlrimportcsvincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportcsvincludes.h
	$(CP) sqlrelay/private/sqlrimportcsv.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportcsv.h

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
	$(RMTREE) $(includedir)/sqlrelay

uninstalldll:
	$(RM) $(bindir)/lib$(SQLR)client.$(LIBEXT) \
		$(libdir)/lib$(SQLR)client.lib \
		$(bindir)/libsqlrclient.$(LIBEXT) \
		$(libdir)/libsqlrclient.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/lib$(SQLR)client.$(LIBEXT) \
				$(libdir)/libsqlrclient.$(LIBEXT)
