top_builddir = ../..

include ../../config.mk

all: oracle.class \
	postgresqlbinds.class \
	run.bat

clean:
	$(RM) *.class cachefile* sqlnet.log run.bat

oracle.class: oracle.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar oracle.java

postgresqlbinds.class: postgresqlbinds.java
	$(JAVAC) -classpath $(javadir)/sqlrelay.jar postgresqlbinds.java

run.bat: run.batch
	$(CP) run.batch run.bat
