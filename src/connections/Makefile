include ../../config.mk

.PHONY: all clean rebuild install uninstall

all:
ifneq ($(strip $(DB2LIBS)),)
	$(MAKE) -C db2 all
endif
ifneq ($(strip $(FREETDSLIBS)),)
	$(MAKE) -C freetds all
endif
ifneq ($(strip $(INTERBASELIBS)),)
	$(MAKE) -C interbase all
endif
ifneq ($(strip $(MDBTOOLSLIBS)),)
	$(MAKE) -C mdbtools all
endif
ifneq ($(strip $(MSQLLIBS)),)
	$(MAKE) -C msql all
endif
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) -C mysql all
endif
ifneq ($(strip $(ODBCLIBS)),)
	$(MAKE) -C odbc all
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) -C oracle7 all
ifneq ($(strip $(ORACLEVERSION)),7)
	$(MAKE) -C oracle8 all
endif
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) -C postgresql all
endif
ifneq ($(strip $(SQLITELIBS)),)
	$(MAKE) -C sqlite all
endif
ifneq ($(strip $(SYBASELIBS)),)
	$(MAKE) -C sybase all
endif
	$(MAKE) -C router all

clean:
	$(MAKE) -C db2 clean
	$(MAKE) -C freetds clean
	$(MAKE) -C interbase clean
	$(MAKE) -C mdbtools clean
	$(MAKE) -C msql clean
	$(MAKE) -C mysql clean
	$(MAKE) -C odbc clean
	$(MAKE) -C oracle7 clean
	$(MAKE) -C oracle8 clean
	$(MAKE) -C postgresql clean
	$(MAKE) -C sqlite clean
	$(MAKE) -C sybase clean
	$(MAKE) -C router clean

rebuild: clean all

install:
ifneq ($(strip $(DB2LIBS)),)
	$(MAKE) -C db2 install
endif
ifneq ($(strip $(FREETDSLIBS)),)
	$(MAKE) -C freetds install
endif
ifneq ($(strip $(INTERBASELIBS)),)
	$(MAKE) -C interbase install
endif
ifneq ($(strip $(MDBTOOLSLIBS)),)
	$(MAKE) -C mdbtools install
endif
ifneq ($(strip $(MSQLLIBS)),)
	$(MAKE) -C msql install
endif
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) -C mysql install
endif
ifneq ($(strip $(ODBCLIBS)),)
	$(MAKE) -C odbc install
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) -C oracle7 install
ifneq ($(strip $(ORACLEVERSION)),7)
	$(MAKE) -C oracle8 install
endif
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) -C postgresql install
endif
ifneq ($(strip $(SQLITELIBS)),)
	$(MAKE) -C sqlite install
endif
ifneq ($(strip $(SYBASELIBS)),)
	$(MAKE) -C sybase install
endif
	$(MAKE) -C router install

uninstall:
	$(MAKE) -C db2 uninstall
	$(MAKE) -C freetds uninstall
	$(MAKE) -C interbase uninstall
	$(MAKE) -C mdbtools uninstall
	$(MAKE) -C msql uninstall
	$(MAKE) -C mysql uninstall
	$(MAKE) -C odbc uninstall
	$(MAKE) -C oracle7 uninstall
	$(MAKE) -C oracle8 uninstall
	$(MAKE) -C postgresql uninstall
	$(MAKE) -C sqlite uninstall
	$(MAKE) -C sybase uninstall
	$(MAKE) -C router uninstall
