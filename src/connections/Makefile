include ../../config.mk

all: $(CONNECTIONSALLSUBDIRS)

all-db2:
	cd db2 $(AND) $(MAKE) all

all-freetds:
	cd freetds $(AND) $(MAKE) all

all-firebird:
	cd firebird $(AND) $(MAKE) all

all-mdbtools:
	cd mdbtools $(AND) $(MAKE) all

all-mysql:
	cd mysql $(AND) $(MAKE) all

all-odbc:
	cd odbc $(AND) $(MAKE) all

all-oracle8:
	cd oracle8 $(AND) $(MAKE) all

all-postgresql:
	cd postgresql $(AND) $(MAKE) all

all-sqlite:
	cd sqlite $(AND) $(MAKE) all

all-sybase:
	cd sybase $(AND) $(MAKE) all

all-router:
	cd router $(AND) $(MAKE) all

clean:
	cd db2 $(AND) $(MAKE) clean
	cd freetds $(AND) $(MAKE) clean
	cd firebird $(AND) $(MAKE) clean
	cd mdbtools $(AND) $(MAKE) clean
	cd mysql $(AND) $(MAKE) clean
	cd odbc $(AND) $(MAKE) clean
	cd oracle8 $(AND) $(MAKE) clean
	cd postgresql $(AND) $(MAKE) clean
	cd sqlite $(AND) $(MAKE) clean
	cd sybase $(AND) $(MAKE) clean
	cd router $(AND) $(MAKE) clean

install: $(CONNECTIONSINSTALLSUBDIRS)

install-db2:
	cd db2 $(AND) $(MAKE) install

install-freetds:
	cd freetds $(AND) $(MAKE) install

install-firebird:
	cd firebird $(AND) $(MAKE) install

install-mdbtools:
	cd mdbtools $(AND) $(MAKE) install

install-mysql:
	cd mysql $(AND) $(MAKE) install

install-odbc:
	cd odbc $(AND) $(MAKE) install

install-oracle8:
	cd oracle8 $(AND) $(MAKE) install

install-postgresql:
	cd postgresql $(AND) $(MAKE) install

install-sqlite:
	cd sqlite $(AND) $(MAKE) install

install-sybase:
	cd sybase $(AND) $(MAKE) install

install-router:
	cd router $(AND) $(MAKE) install

uninstall:
	cd db2 $(AND) $(MAKE) uninstall
	cd freetds $(AND) $(MAKE) uninstall
	cd firebird $(AND) $(MAKE) uninstall
	cd mdbtools $(AND) $(MAKE) uninstall
	cd mysql $(AND) $(MAKE) uninstall
	cd odbc $(AND) $(MAKE) uninstall
	cd oracle8 $(AND) $(MAKE) uninstall
	cd postgresql $(AND) $(MAKE) uninstall
	cd sqlite $(AND) $(MAKE) uninstall
	cd sybase $(AND) $(MAKE) uninstall
	cd router $(AND) $(MAKE) uninstall
