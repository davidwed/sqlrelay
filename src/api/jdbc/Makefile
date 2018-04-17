top_builddir = ../../..

include ../../../config.mk

SRCS = com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayConnection.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayDriver.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayDatabaseMetaData.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayStatement.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayParameter.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayParameterMetaData.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayPreparedStatement.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayCallableStatement.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayResultSet.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayResultSetMetaData.java \
	com$(SLASH)firstworks$(SLASH)sql$(SLASH)SQLRelayConnectInfo.java

all: sqlrelayjdbc.jar

clean:
	$(RM) com/firstworks/sql/*.class *.jar

sqlrelayjdbc.jar: $(SRCS)
	$(JAVAC) -classpath ../java/sqlrelay.jar $(SRCS)
	$(JAR) -cf $@ META-INF com

install:
	$(CP) sqlrelayjdbc.jar $(javadir)
	$(CHMOD) 644 $(javadir)/sqlrelayjdbc.jar

uninstall:
	$(RM) $(javadir)/sqlrelayjdbc.jar
