top_builddir = ../..

include ../../config.mk

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

clean:
	$(LIBTOOL) --mode=clean $(RM) *.class
	$(LIBTOOL) --mode=clean $(RM) cachefile*
	$(LIBTOOL) --mode=clean $(RM) sqlnet.log

db2.class: db2.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) db2.java

freetds.class: freetds.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) freetds.java

interbase.class: interbase.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) interbase.java

lago.class: lago.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) lago.java

msql.class: msql.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) msql.java

mysql.class: mysql.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) mysql.java

oracle7.class: oracle7.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) oracle7.java

oracle8.class: oracle8.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) oracle8.java

oracle8i.class: oracle8i.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) oracle8i.java

postgresql6.class: postgresql6.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) postgresql6.java

postgresql7.class: postgresql7.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) postgresql7.java

sqlite.class: sqlite.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) sqlite.java

sybase.class: sybase.java
	$(JAVAC) -classpath $$CLASSPATH:$(javadir) sybase.java


rebuild.class: clean all
