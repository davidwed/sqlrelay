top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(TCLCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(TCLCPPFLAGS) $(COMPILE) $<
	

all: sqlrelay.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.lo *.o *.obj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.ii
	$(RMTREE) .libs

sqlrelay.$(LIBEXT): sqlrelayCmd.cpp sqlrelayCmd.$(OBJ)
	$(LTLINK) $(LINK) $(OUT)$@ sqlrelayCmd.$(OBJ) $(LDFLAGS) $(TCLLIBS) $(MODLINKFLAGS)

install: $(INSTALLLIB)
	$(CP) pkgIndex.tcl $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(CHMOD) 644 $(DESTDIR)$(TCLLIBSPATH)/sqlrelay/pkgIndex.tcl

installdll:
	$(MKINSTALLDIRS) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(LTINSTALL) $(CP) sqlrelay.$(LIBEXT) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay

installlib:
	$(MKINSTALLDIRS) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(LTINSTALL) $(CP) sqlrelay.$(LIBEXT) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
	$(RM) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay/sqlrelay.$(LIBEXT)
	$(RM) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay/sqlrelay.a

uninstall:
	$(RMTREE) $(DESTDIR)$(TCLLIBSPATH)/sqlrelay
