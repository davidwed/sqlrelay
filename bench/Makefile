include ../config.mk

top_srcdir = ..

all: 
	if ( test -n "$(ORACLELIBS)" ); then \
		cd oracle7; $(MAKE) all; \
	fi
	if ( test -n "$(ORACLELIBS)" -a "$(ORACLEVERSION)" != "7" ); then \
		cd oracle8; $(MAKE) all; \
	fi
	if ( test -n "$(MYSQLLIBS)" ); then \
		cd mysql; $(MAKE) all; \
	fi
	if ( test -n "$(MSQLLIBS)" ); then \
		cd msql; $(MAKE) all; \
	fi
	if ( test -n "$(POSTGRESQLLIBS)" ); then \
		cd postgresql; $(MAKE) all; \
	fi
	if ( test -n "$(SQLITELIBS)" ); then \
		cd sqlite; $(MAKE) all; \
	fi
	if ( test -n "$(LAGOLIBS)" ); then \
		cd lago; $(MAKE) all; \
	fi
	if ( test -n "$(FREETDSLIBS)" ); then \
		cd freetds; $(MAKE) all; \
	fi
	if ( test -n "$(ODBCLIBS)" ); then \
		cd odbc; $(MAKE) all; \
	fi
	if ( test -n "$(DB2LIBS)" ); then \
		cd db2; $(MAKE) all; \
	fi
	if ( test -n "$(INTERBASELIBS)" ); then \
		cd interbase; $(MAKE) all; \
	fi
	if ( test -n "$(SYBASELIBS)" ); then \
		cd sybase; $(MAKE) all; \
	fi
	cd sqlrelay; $(MAKE) all

clean:
	cd oracle7; $(MAKE) clean
	cd oracle8; $(MAKE) clean
	cd mysql; $(MAKE) clean
	cd msql; $(MAKE) clean
	cd postgresql; $(MAKE) clean
	cd sqlite; $(MAKE) clean
	cd lago; $(MAKE) clean
	cd freetds; $(MAKE) clean
	cd odbc; $(MAKE) clean
	cd db2; $(MAKE) clean
	cd sqlrelay; $(MAKE) clean
	cd interbase; $(MAKE) clean
	cd sybase; $(MAKE) clean

rebuild: clean all
