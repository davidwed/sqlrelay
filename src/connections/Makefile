include ../../config.mk

top_srcdir = ../..

all:
	if ( test -n "$(DB2LIBS)" ); then \
		cd db2; $(MAKE) all; \
	fi
	if ( test -n "$(FREETDSLIBS)" ); then \
		cd freetds; $(MAKE) all; \
	fi
	if ( test -n "$(INTERBASELIBS)" ); then \
		cd interbase; $(MAKE) all; \
	fi
	if ( test -n "$(LAGOLIBS)" ); then \
		cd lago; $(MAKE) all; \
	fi
	if ( test -n "$(MSQLLIBS)" ); then \
		cd msql; $(MAKE) all; \
	fi
	if ( test -n "$(MYSQLLIBS)" ); then \
		cd mysql; $(MAKE) all; \
	fi
	if ( test -n "$(ODBCLIBS)" ); then \
		cd odbc; $(MAKE) all; \
	fi
	if ( test -n "$(ORACLELIBS)" ); then \
		cd oracle7; $(MAKE) all; \
	fi
	if ( test -n "$(ORACLELIBS)" -a "$(ORACLEVERSION)" != "7" ); then \
		cd oracle8; $(MAKE) all; \
	fi
	if ( test -n "$(POSTGRESQLLIBS)" ); then \
		cd postgresql; $(MAKE) all; \
	fi
	if ( test -n "$(SQLITELIBS)" ); then \
		cd sqlite; $(MAKE) all; \
	fi
	if ( test -n "$(SYBASELIBS)" ); then \
		cd sybase; $(MAKE) all; \
	fi

clean:
	cd oracle7; $(MAKE) clean
	cd oracle8; $(MAKE) clean
	cd mysql; $(MAKE) clean
	cd msql; $(MAKE) clean
	cd postgresql; $(MAKE) clean
	cd sqlite; $(MAKE) clean
	cd lago; $(MAKE) clean
	cd freetds; $(MAKE) clean
	cd sybase; $(MAKE) clean
	cd odbc; $(MAKE) clean
	cd db2; $(MAKE) clean
	cd interbase; $(MAKE) clean

rebuild: clean all

install:
	if ( test -n "$(ORACLELIBS)" ); then \
		cd oracle7; $(MAKE) install; \
	fi
	if ( test -n "$(ORACLELIBS)" -a "$(ORACLEVERSION)" != "7"); then \
		cd oracle8; $(MAKE) install; \
	fi
	if ( test -n "$(MYSQLLIBS)" ); then \
		cd mysql; $(MAKE) install; \
	fi
	if ( test -n "$(MSQLLIBS)" ); then \
		cd msql; $(MAKE) install; \
	fi
	if ( test -n "$(POSTGRESQLLIBS)" ); then \
		cd postgresql; $(MAKE) install; \
	fi
	if ( test -n "$(SQLITELIBS)" ); then \
		cd sqlite; $(MAKE) install; \
	fi
	if ( test -n "$(LAGOLIBS)" ); then \
		cd lago; $(MAKE) install; \
	fi
	if ( test -n "$(FREETDSLIBS)" ); then \
		cd freetds; $(MAKE) install; \
	fi
	if ( test -n "$(SYBASELIBS)" ); then \
		cd sybase; $(MAKE) install; \
	fi
	if ( test -n "$(ODBCLIBS)" ); then \
		cd odbc; $(MAKE) install; \
	fi
	if ( test -n "$(DB2LIBS)" ); then \
		cd db2; $(MAKE) install; \
	fi
	if ( test -n "$(INTERBASELIBS)" ); then \
		cd interbase; $(MAKE) install; \
	fi

uninstall:
	cd oracle7; $(MAKE) uninstall
	cd oracle8; $(MAKE) uninstall
	cd mysql; $(MAKE) uninstall
	cd msql; $(MAKE) uninstall
	cd postgresql; $(MAKE) uninstall
	cd sqlite; $(MAKE) uninstall
	cd lago; $(MAKE) uninstall
	cd freetds; $(MAKE) uninstall
	cd sybase; $(MAKE) uninstall
	cd odbc; $(MAKE) uninstall
	cd db2; $(MAKE) uninstall
	cd interbase; $(MAKE) uninstall
