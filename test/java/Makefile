include ../../config.mk

clean:
	$(LIBTOOL) --mode=clean $(RM) *.class

all:
ifneq ($(strip $(DB2LIBS)),)
	$(MAKE) db2
endif
ifneq ($(strip $(FREETDSLIBS)),)
	$(MAKE) freetds
endif
ifneq ($(strip $(INTERBASELIBS)),)
	$(MAKE) interbase
endif
ifneq ($(strip $(LAGOLIBS)),)
	$(MAKE) lago
endif
ifneq ($(strip $(MSQLLIBS)),)
	$(MAKE) msql
endif
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) mysql
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) oracle7
	$(MAKE) oracle8
	$(MAKE) oracle8i
endif
ifneq ($(strip $(POSTGRESQLLIBS)),)
	$(MAKE) postgresql6
	$(MAKE) postgresql7
endif
ifneq ($(strip $(SQLITELIBS)),)
	$(MAKE) sqlite
endif
ifneq ($(strip $(SYBASELIBS)),)
	$(MAKE) sybase
endif

db2.class: db2.java
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java db2.java

freetds.class: freetds.c freetds.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java freetds.java

interbase.class: interbase.c interbase.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java interbase.java

lago.class: lago.c lago.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java lago.java

msql.class: msql.c msql.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java msql.java

mysql.class: mysql.c mysql.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java mysql.java

oracle7.class: oracle7.c oracle7.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java oracle7.java

oracle8.class: oracle8.c oracle8.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java oracle8.java

oracle8i.class: oracle8i.c oracle8i.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java oracle8i.java

postgresql6.class: postgresql6.c postgresql6.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java postgresql6.java

postgresql7.class: postgresql7.c postgresql7.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java postgresql7.java

sqlite.class: sqlite.c sqlite.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java sqlite.java

sybase.class: sybase.c sybase.lo
	javac -classpath $$CLASSPATH:/usr/local/firstworks/java sybase.java


rebuild.class: clean all
