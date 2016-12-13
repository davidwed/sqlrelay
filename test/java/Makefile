top_builddir = ../..

include ../../config.mk

all: db2.class\
	freetds.class \
	firebird.class \
	mysql.class \
	oracle.class \
	postgresql.class \
	sqlite.class \
	sap.class \
	krb.class \
	tls.class \
	run.bat

clean:
	$(RM) *.class cachefile* sqlnet.log run.bat

db2.class: db2.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar db2.java

freetds.class: freetds.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar freetds.java

firebird.class: firebird.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar firebird.java

mysql.class: mysql.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar mysql.java

oracle.class: oracle.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar oracle.java

postgresql.class: postgresql.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar postgresql.java

sqlite.class: sqlite.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar sqlite.java

sap.class: sap.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar sap.java

tls.class: tls.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar tls.java

krb.class: krb.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar krb.java

run.bat: run.batch
	$(CP) run.batch run.bat
