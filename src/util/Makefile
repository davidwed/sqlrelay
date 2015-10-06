top_builddir = ../..

include ../../config.mk

SRCS = sqlrcmdline.cpp \
	sqlrpaths.cpp \
	sqlrconfig.cpp \
	sqlrconfigs.cpp \
	$(STATICUTILPLUGINSRCS)

LOBJS = sqlrcmdline.$(OBJ) \
	sqlrpaths.$(OBJ) \
	sqlrconfig.$(OBJ) \
	sqlrconfigs.$(OBJ)

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(UTILCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(UTILCPPFLAGS) $(COMPILE) $<

all: $(STATICUTILPLUGINSRCS) libsqlrutil.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest $(STATICUTILPLUGINSRCS)
	$(RMTREE) .libs

libsqlrutil.$(LIBEXT): $(SRCS) $(LOBJS)
	$(LTLINK) $(LINK) $(OUT)$@ $(LOBJS) $(STATICUTILPLUGINLOBJS) $(LDFLAGS) $(LINKFLAGS) $(UTILLIBS)

sqlrconfigdeclarations.cpp: sqlrconfigdeclarations.cpp.in
	$(RM) $@
	for file in `ls ../configs/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

sqlrconfigassignments.cpp: sqlrconfigassignments.cpp.in
	$(RM) $@
	for file in `ls ../configs/*.$(OBJ)`; \
	do \
		module=`basename $$file .$(OBJ)`; \
		sed -e "s|@module@|$$module|g" $< >> $@; \
	done

install: $(INSTALLLIB)
	$(MKINSTALLDIRS) $(includedir)/sqlrelay
	$(CP) sqlrelay/sqlrutil.h $(includedir)/sqlrelay
	$(CHMOD) 644 $(includedir)/sqlrelay/sqlrutil.h
	$(MKINSTALLDIRS) $(includedir)/sqlrelay/private
	$(CP) sqlrelay/private/sqlrutildll.h $(includedir)/sqlrelay/private
	$(CHMOD) 644 $(includedir)/sqlrelay/private/sqlrutildll.h

installdll:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) $(CP) libsqlrutil.$(LIBEXT) $(bindir)
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrutil.lib $(libdir)

installlib:
	$(MKINSTALLDIRS) $(libdir)
	$(LTINSTALL) $(CP) libsqlrutil.$(LIBEXT) $(libdir)
	$(LTFINISH) $(libdir)

uninstall: $(UNINSTALLLIB)
	$(RM) $(includedir)/sqlrelay/sqlrutil.h \
		$(includedir)/sqlrelay/private/sqlrutildll.h

uninstalldll:
	$(RM) $(bindir)/libsqlrutil.$(LIBEXT) \
		$(libdir)/libsqlrutil.lib

uninstalllib:
	$(LTUNINSTALL) $(RM) $(libdir)/libsqlrutil.$(LIBEXT)
