top_builddir = ../../..

include ../../../config.mk

SRCS = SQLRelayConnection.java \
	SQLRelayDriver.java \
	SQLRelayDatabaseMetaData.java \
	SQLRelayStatement.java \
	SQLRelayPreparedStatement.java \
	SQLRelayCallableStatement.java \
	SQLRelayResultSet.java \
	SQLRelayResultSetMetaData.java \
	SQLRelayConnectInfo.java

OBJS = SQLRelayConnection.class \
	SQLRelayDriver.class \
	SQLRelayDatabaseMetaData.class \
	SQLRelayStatement.class \
	SQLRelayPreparedStatement.class \
	SQLRelayCallableStatement.class \
	SQLRelayResultSet.class \
	SQLRelayResultSetMetaData.class \
	SQLRelayConnectInfo.class

all: sqlrelayjdbc.jar

clean:
	$(LTCLEAN) $(RM) *.class *.jar

sqlrelayjdbc.jar: $(SRCS)
	$(JAVAC) -cp ../java/sqlrelay.jar $(SRCS)
	$(JAR) -cf $@ $(OBJS)

install:
	$(CP) sqlrelayjdbc.jar $(javadir)
	$(CHMOD) 644 $(javadir)/sqlrelayjdbc.jar

uninstall:
	$(RM) $(javadir)/sqlrelayjdbc.jar
