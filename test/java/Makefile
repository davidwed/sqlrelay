top_builddir = ../..

include ../../config.mk

.PHONY: all clean rebuild.class

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
	$(MAKE) oracle8.class
	$(MAKE) oracle8i.class
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql6.class
	$(MAKE) postgresql7.class
endif
ifneq ($(strip $(SQLITELIBS)),)
	$(MAKE) sqlite.class
endif
ifneq ($(strip $(SYBASELIBS)),)
	$(MAKE) sybase.class
endif

clean:
	$(RM) *.class
	$(RM) cachefile*
	$(RM) sqlnet.log

db2.class: db2.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar db2.java

freetds.class: freetds.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar freetds.java

firebird.class: firebird.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar firebird.java

mysql.class: mysql.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar mysql.java

oracle8.class: oracle8.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar oracle8.java

oracle8i.class: oracle8i.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar oracle8i.java

postgresql6.class: postgresql6.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar postgresql6.java

postgresql7.class: postgresql7.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar postgresql7.java

sqlite.class: sqlite.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar sqlite.java

sybase.class: sybase.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar sybase.java

test.class: test.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir)/sqlrelay.jar test.java

rebuild.class: clean all
