top_builddir = ../../..

include ../../../config.mk

SRCS = sqlrconnection.cpp \
	sqlrcursor.cpp \
	sqlrresultsetdomnode.cpp \
	sqlrimport.cpp \
	sqlrimportxml.cpp \
	sqlrimportcsv.cpp \
	sqlrexport.cpp \
	sqlrexportxml.cpp \
	sqlrexportcsv.cpp
LOBJS = sqlrconnection.$(OBJ) \
	sqlrcursor.$(OBJ) \
	sqlrresultsetdomnode.$(OBJ) \
	sqlrimport.$(OBJ) \
	sqlrimportxml.$(OBJ) \
	sqlrimportcsv.$(OBJ) \
	sqlrexport.$(OBJ) \
	sqlrexportxml.$(OBJ) \
	sqlrexportcsv.$(OBJ)

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
	$(CP) sqlrelay/sqlrimport.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrimport.h
	$(CP) sqlrelay/sqlrimportcsv.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrimportcsv.h
	$(CP) sqlrelay/sqlrimportxml.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrimportxml.h
	$(CP) sqlrelay/sqlrexport.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrexport.h
	$(CP) sqlrelay/sqlrexportcsv.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrexportcsv.h
	$(CP) sqlrelay/sqlrexportxml.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrexportxml.h
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
	$(CP) sqlrelay/private/sqlrimportincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportincludes.h
	$(CP) sqlrelay/private/sqlrimport.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimport.h
	$(CP) sqlrelay/private/sqlrimportcsvincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportcsvincludes.h
	$(CP) sqlrelay/private/sqlrimportcsv.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportcsv.h
	$(CP) sqlrelay/private/sqlrimportxmlincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportxmlincludes.h
	$(CP) sqlrelay/private/sqlrimportxml.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrimportxml.h
	$(CP) sqlrelay/private/sqlrexportincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrexportincludes.h
	$(CP) sqlrelay/private/sqlrexport.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrexport.h
	$(CP) sqlrelay/private/sqlrexportcsvincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrexportcsvincludes.h
	$(CP) sqlrelay/private/sqlrexportcsv.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrexportcsv.h
	$(CP) sqlrelay/private/sqlrexportxmlincludes.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrexportxmlincludes.h
	$(CP) sqlrelay/private/sqlrexportxml.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrexportxml.h

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
