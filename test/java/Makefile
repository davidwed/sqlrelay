top_builddir = ../..

include ../../config.mk

clean:
	$(LIBTOOL) --mode=clean $(RM) *.class
	$(LIBTOOL) --mode=clean $(RM) cachefile*
	$(LIBTOOL) --mode=clean $(RM) sqlnet.log

all:
ifneq ($(strip $(DB2LIBS)),)
	$(MAKE) db2.class
endif
ifneq ($(strip $(FREETDSLIBS)),)
	$(MAKE) freetds.class
endif
ifneq ($(strip $(INTERBASELIBS)),)
	$(MAKE) interbase.class
endif
ifneq ($(strip $(LAGOLIBS)),)
	$(MAKE) lago.class
endif
ifneq ($(strip $(MSQLLIBS)),)
	$(MAKE) msql.class
endif
ifneq ($(strip $(MYSQLLIBS)),)
	$(MAKE) mysql.class
endif
ifneq ($(strip $(ORACLELIBS)),)
	$(MAKE) oracle7.class
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

db2.class: db2.java
	javac -classpath $$CLASSPATH:$(javadir) db2.java

freetds.class: freetds.java
	javac -classpath $$CLASSPATH:$(javadir) freetds.java

interbase.class: interbase.java
	javac -classpath $$CLASSPATH:$(javadir) interbase.java

lago.class: lago.java
	javac -classpath $$CLASSPATH:$(javadir) lago.java

msql.class: msql.java
	javac -classpath $$CLASSPATH:$(javadir) msql.java

mysql.class: mysql.java
	javac -classpath $$CLASSPATH:$(javadir) mysql.java

oracle7.class: oracle7.java
	javac -classpath $$CLASSPATH:$(javadir) oracle7.java

oracle8.class: oracle8.java
	javac -classpath $$CLASSPATH:$(javadir) oracle8.java

oracle8i.class: oracle8i.java
	javac -classpath $$CLASSPATH:$(javadir) oracle8i.java

postgresql6.class: postgresql6.java
	javac -classpath $$CLASSPATH:$(javadir) postgresql6.java

postgresql7.class: postgresql7.java
	javac -classpath $$CLASSPATH:$(javadir) postgresql7.java

sqlite.class: sqlite.java
	javac -classpath $$CLASSPATH:$(javadir) sqlite.java

sybase.class: sybase.java
	javac -classpath $$CLASSPATH:$(javadir) sybase.java


rebuild.class: clean all
