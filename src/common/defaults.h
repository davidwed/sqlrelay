// sqlr-connection/sqlr-listener parameters

#define DEFAULT_ID "defaultid"
	// default id for listeners/connections

#define DEFAULT_PORT "9000"
	// default port to listen on

#define DEFAULT_SOCKET "/tmp/sqlrelay.socket"
	// default unix socket to listen on

#define DEFAULT_DBASE "oracle8"
	// default database type

#define DEFAULT_CONNECTIONS "1"
	// default number of connections to start

#define DEFAULT_ENDOFSESSION "commit"
	// default action to take at end of session

#define DEFAULT_MAXCONNECTIONS "1"
	// default maximum number of connections

#define DEFAULT_MAXQUEUELENGTH "1"
	// default maximum queue length before
	// another connection will be fired off

#define DEFAULT_GROWBY "1"
	// default number of connections to grow by

#define DEFAULT_TTL "60"
	// default time to live for idle connections
	// that were fired off to handle increased load

#define DEFAULT_SESSIONTIMEOUT "60"
	// default session timeout

#define DEFAULT_RUNASUSER "nobody"
	// default user to run as

#define DEFAULT_RUNASGROUP "nobody"
	// default group to run as

#define DEFAULT_CURSORS "5"
	// default number of cursors to open

#define DEFAULT_AUTHTIER "listener"
	// default tier to authenticate users on

#define DEFAULT_HANDOFF "reconnect"
	// default method to use for handing off
	// clients from listener to connection

#define DEFAULT_ALLOWEDIPS ""
	// default regular expression for IP's that are allowed to connect

#define DEFAULT_DENIEDIPS ""
	// default regular expression for IP's that are not allowed to connect

#define DEFAULT_DEBUG "none"
	// default tiers to debug on

#define DEFAULT_USER "user"
	// default user name for authentication

#define DEFAULT_PASSWORD "password"
	// default password for authentication

#define DEFAULT_CONNECTIONID "defaultid"
	// default id for an individual set of connections

#define DEFAULT_CONNECTSTRING "user=scott;password=tiger;"
	// default connect string

#define DEFAULT_METRIC "1"
	// default metric

#define DEFAULT_IDLECLIENTTIMEOUT -1



// cachemanager/sqlrclient parameters
#define DEFAULT_INTERVAL 30
	// default interval that the cachemanager will scan on



// sqlr-start parameters
#define MAX_CONNECTIONS 200
	// maximum number of connections that may be started using sqlr-start
