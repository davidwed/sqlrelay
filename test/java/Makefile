top_builddir = ../..

include ../../config.mk

.PHONY: all clean

all:
ifneq ($(strip $(DB2LIBS)),)
	$(MAKE) db2.class
endif
ifneq ($(strip $(FREETDSLIBS)),)
	$(MAKE) freetds.class
endif
ifneq ($(strip $(FIREBIRDLIBS)),)
	$(MAKE) firebird.class
endif
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) mysql.class
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) oracle.class
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql.class
endif
ifneq ($(strip $(SQLITELIBS)),)
	$(MAKE) sqlite.class
endif
ifneq ($(strip $(SYBASELIBS)),)
	$(MAKE) sybase.class
endif

clean:
	$(RM) *.class cachefile* sqlnet.log

db2.class: db2.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar db2.java

freetds.class: freetds.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar freetds.java

firebird.class: firebird.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar firebird.java

mysql.class: mysql.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar mysql.java

oracle.class: oracle.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar oracle.java

postgresql.class: postgresql.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar postgresql.java

sqlite.class: sqlite.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar sqlite.java

sybase.class: sybase.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar sybase.java

test.class: test.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar test.java
