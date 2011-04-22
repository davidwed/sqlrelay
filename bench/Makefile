include ../config.mk

.PHONY: all clean

all: 
ifneq ($(ORACLELIBS),)
	$(MAKE) -C oracle8 all
endif
ifneq ($(MYSQLLIBS),)
	$(MAKE) -C mysql all
endif
ifneq ($(POSTGRESQLLIBS),)
	$(MAKE) -C postgresql all
endif
ifneq ($(SQLITELIBS),)
	$(MAKE) -C sqlite all
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
ifneq ($(FIREBIRDLIBS),)
	$(MAKE) -C firebird all
endif
ifneq ($(SYBASELIBS),)
	$(MAKE) -C sybase all
endif
	$(MAKE) -C sqlrelay all

clean:
	$(MAKE) -C oracle8 clean
	$(MAKE) -C mysql clean
	$(MAKE) -C postgresql clean
	$(MAKE) -C sqlite clean
	$(MAKE) -C freetds clean
	$(MAKE) -C odbc clean
	$(MAKE) -C db2 clean
	$(MAKE) -C firebird clean
	$(MAKE) -C sybase clean
	$(MAKE) -C sqlrelay clean
