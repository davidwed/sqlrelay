include ../config.mk

all: 
ifneq ($(ORACLELIBS),)
	$(MAKE) -C oracle7 all
	ifneq ($(ORACLELIBS),7)
		$(MAKE) -C oracle8 all
	endif
endif
ifneq ($(MYSQLLIBS),)
	$(MAKE) -C mysql all
endif
ifneq ($(MSQLLIBS),)
	$(MAKE) -C msql all
endif
ifneq ($(POSTGRESQLLIBS),)
	$(MAKE) -C postgresql all
endif
ifneq ($(SQLITELIBS),)
	$(MAKE) -C sqlite all
endif
ifneq ($(LAGOLIBS),)
	$(MAKE) -C lago all
endif
ifneq ($(FREETDSLIBS),)
	$(MAKE) -C freetds all
endif
ifneq ($(ODBCLIBS),)
	$(MAKE) -C odbc all
endif
ifneq ($(DB2LIBS),)
	$(MAKE) -C db2 all
endif
ifneq ($(INTERBASELIBS),)
	$(MAKE) -C interbase all
endif
ifneq ($(SYBASELIBS),)
	$(MAKE) -C sybase all
endif
	$(MAKE) -C sqlrelay all

clean:
	$(MAKE) -C oracle7 clean
	$(MAKE) -C oracle8 clean
	$(MAKE) -C mysql clean
	$(MAKE) -C msql clean
	$(MAKE) -C postgresql clean
	$(MAKE) -C sqlite clean
	$(MAKE) -C lago clean
	$(MAKE) -C freetds clean
	$(MAKE) -C odbc clean
	$(MAKE) -C db2 clean
	$(MAKE) -C interbase clean
	$(MAKE) -C sybase clean
	$(MAKE) -C sqlrelay clean

rebuild: clean all
