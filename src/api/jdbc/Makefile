top_builddir = ../../..

include ../../../config.mk

SRCS = SQLRelayConnection.java \
	SQLRelayDriver.java \
	SQLRelayDatabaseMetaData.java \
	SQLRelayStatement.java \
	SQLRelayPreparedStatement.java \
	SQLRelayCallableStatement.java \
	SQLRelayResultSet.java \
	SQLRelayResultSetMetaData.java

OBJS = SQLRelayConnection.class \
	SQLRelayDriver.class \
	SQLRelayDatabaseMetaData.class \
	SQLRelayStatement.class \
	SQLRelayPreparedStatement.class \
	SQLRelayCallableStatement.class \
	SQLRelayResultSet.class \
	SQLRelayResultSetMetaData.class

all: sqlr.jar

clean:
	$(LTCLEAN) $(RM) *.class *.jar

sqlr.jar: $(SRCS)
	$(JAVAC) $(SRCS)
	$(JAR) -cf $@ $(OBJS)

install:
	$(CP) sqlr.jar $(javadir)
	$(CHMOD) 644 $(javadir)/sqlr.jar

uninstall:
	$(RM) $(javadir)/sqlr.jar
