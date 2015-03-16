top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo .xs

.xs.cpp:
	$(PERL) $(XSUBPP) -C++ -typemap $(PERLLIB)/ExtUtils/typemap -typemap perlobject.map -typemap typemap $< > $<c
	$(MV) $<c $@

.cpp.lo:
	$(LTCOMPILE) $(CXX) $(CXXFLAGS) $(PERLCPPFLAGS) $(COMPILE) $< $(OUT)$@

.cpp.obj:
	$(CXX) $(CXXFLAGS) $(PERLCPPFLAGS) $(COMPILE) $<

all: Connection.$(LIBEXT) Cursor.$(LIBEXT)

clean:
	$(LTCLEAN) $(RM) *.cpp *.lo *.o *.odj *.$(LIBEXT) *.lib *.exp *.idb *.pdb *.manifest *.so *.$(PERLMAN3EXT_LOCAL)
	$(RMTREE) .libs

Connection.$(LIBEXT): Connection.xs Connection.cpp Connection.$(OBJ)
	$(LTLINK) $(CXX) $(OUT)$@ Connection.$(OBJ) $(LDFLAGS) $(PERLCONLIBS) $(PERLRPATH) $(MODLINKFLAGS)

Cursor.$(LIBEXT): Cursor.xs Cursor.cpp Cursor.$(OBJ)
	$(LTLINK) $(CXX) $(OUT)$@ Cursor.$(OBJ) $(LDFLAGS) $(PERLCURLIBS) $(PERLRPATH) $(MODLINKFLAGS)


install: $(INSTALLLIB)
	$(CP) Connection.pm $(PERLSITEARCH_LOCAL)/SQLRelay
	$(CHMOD) 644 $(PERLSITEARCH_LOCAL)/SQLRelay/Connection.pm
	$(CP) Cursor.pm $(PERLSITEARCH_LOCAL)/SQLRelay
	$(CHMOD) 644 $(PERLSITEARCH_LOCAL)/SQLRelay/Cursor.pm
	$(MKINSTALLDIRS) $(PERLSITELIB_LOCAL)/DBD
	$(CP) SQLRelay.pm $(PERLSITELIB_LOCAL)/DBD
	$(CHMOD) 644 $(PERLSITELIB_LOCAL)/DBD/SQLRelay.pm
	touch $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.bs
	touch $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.bs
	echo $(PERLSITEARCH_LOCAL)/SQLRelay/Connection.pm > $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH_LOCAL)/SQLRelay/Cursor.pm > $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.$(MODULESUFFIX) >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.$(MODULESUFFIX) >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.bs >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	echo $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.bs >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLSITELIB_LOCAL)/DBD/SQLRelay.pm >> $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay/.packlist
ifneq ($(POD2MAN),)
ifneq ($(PERLINSTALLMAN3DIR_LOCAL),)
	$(POD2MAN) Connection.pm > SQLRelay::Connection.$(PERLMAN3EXT_LOCAL)
	$(POD2MAN) Cursor.pm > SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL)
	$(POD2MAN) SQLRelay.pm > DBD::SQLRelay.$(PERLMAN3EXT_LOCAL)
	$(MKINSTALLDIRS) $(PERLINSTALLMAN3DIR_LOCAL)
	$(CP) SQLRelay::Connection.$(PERLMAN3EXT_LOCAL) $(PERLINSTALLMAN3DIR_LOCAL)
	$(CHMOD) 644 $(PERLINSTALLMAN3DIR_LOCAL)/SQLRelay::Connection.$(PERLMAN3EXT_LOCAL)
	$(CP) SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL) $(PERLINSTALLMAN3DIR_LOCAL)
	$(CHMOD) 644 $(PERLINSTALLMAN3DIR_LOCAL)/SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL)
	$(CP) DBD::SQLRelay.$(PERLMAN3EXT_LOCAL) $(PERLINSTALLMAN3DIR_LOCAL)
	$(CHMOD) 644 $(PERLINSTALLMAN3DIR_LOCAL)/DBD::SQLRelay.$(PERLMAN3EXT_LOCAL)
	$(RM) SQLRelay::Connection.$(PERLMAN3EXT_LOCAL)
	$(RM) SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL)
	$(RM) DBD::SQLRelay.$(PERLMAN3EXT_LOCAL)
	echo $(PERLINSTALLMAN3DIR_LOCAL)/SQLRelay::Connection.$(PERLMAN3EXT_LOCAL) >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	echo $(PERLINSTALLMAN3DIR_LOCAL)/SQLRelay::Cursor.$(PERLMAN3EXT_LOCAL) >> $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	echo $(PERLINSTALLMAN3DIR_LOCAL)/DBD::SQLRelay.$(PERLMAN3EXT_LOCAL) >> $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay/.packlist
endif
endif

installdll:
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/SQLRelay
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay
	$(LTINSTALL) $(CP) Connection.$(LIBEXT) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.$(MODULESUFFIX)
	$(CHMOD) 755 $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.$(MODULESUFFIX)
	$(LTINSTALL) $(CP) Cursor.$(LIBEXT) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.$(MODULESUFFIX)
	$(CHMOD) 755 $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.$(MODULESUFFIX)

installlib:
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/SQLRelay
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor
	$(MKINSTALLDIRS) $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay
	$(LTINSTALL) $(CP) Connection.$(LIBEXT) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection
	$(CHMOD) 755 $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.$(LIBEXT)
	$(LTINSTALL) $(CP) Cursor.$(LIBEXT) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor
	$(CHMOD) 755 $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.$(LIBEXT)
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.a
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.$(LIBEXT)
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.a
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.$(LIBEXT)
	$(MODULERENAME) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/Connection.so so $(MODULESUFFIX)
	$(MODULERENAME) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/Cursor.so so $(MODULESUFFIX)

uninstall:
	$(RM) `cat $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist`
	$(RM) `cat $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist`
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Connection/.packlist
	$(RM) $(PERLSITEARCH_LOCAL)/auto/SQLRelay/Cursor/.packlist
	$(RM) `cat $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay/.packlist`
	$(RM) $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay/.packlist
	$(RMTREE) $(PERLSITEARCH_LOCAL)/auto/SQLRelay
	$(RMTREE) $(PERLSITEARCH_LOCAL)/auto/DBD/SQLRelay
	$(RMTREE) $(PERLSITEARCH_LOCAL)/SQLRelay
