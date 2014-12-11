// sqlr-connection/sqlr-listener parameters

#define DEFAULT_ID "defaultid"
	// default id for listeners/connections

#define DEFAULT_ADDRESS "0.0.0.0"
	// default addresses for listeners/connections

#define DEFAULT_PORT "9000"
	// default port to listen on

#define DEFAULT_PROTOCOL "sqlrclient"
	// default client-server protocol

#define DEFAULT_DBASE "oracle8"
	// default database type

#define DEFAULT_CONNECTIONS "5"
	// default number of connections to start

#define DEFAULT_ENDOFSESSION "commit"
	// default action to take at end of session

#define DEFAULT_MAXQUEUELENGTH "0"
	// default maximum queue length before
	// another connection will be fired off

#define DEFAULT_GROWBY "1"
	// default number of connections to grow by

#define DEFAULT_TTL "60"
	// default time to live for idle connections
	// that were fired off to handle increased load

#define DEFAULT_MAXSESSIONCOUNT "0"
	// default max client sessions for connections
	// that were fired off to handle increased load

#define DEFAULT_SESSIONTIMEOUT "600"
	// default session timeout

#define DEFAULT_RUNASUSER "nobody"
	// default user to run as

#define DEFAULT_RUNASGROUP "nobody"
	// default group to run as

#define DEFAULT_CURSORS "1"
	// default number of cursors to open

#define DEFAULT_CURSORS_GROWBY "1"
	// when we need more cursors, allocate this many more in one shot

#define DEFAULT_AUTHTIER "connection"
	// default tier to authenticate users on

#define DEFAULT_SESSION_HANDLER "thread"
	// default method to use for handing client sessions

#define DEFAULT_HANDOFF "pass"
	// default method to use for handing off
	// clients from listener to connection

#define DEFAULT_ALLOWEDIPS ""
	// default regular expression for IP's that are allowed to connect

#define DEFAULT_DENIEDIPS ""
	// default regular expression for IP's that are not allowed to connect

#define DEFAULT_DEBUG "none"
	// default tiers to debug on

#define DEFAULT_MAXCLIENTINFOLENGTH "512"
	// default max client info length

#define DEFAULT_MAXQUERYSIZE "65536"
	// default max query size

#define DEFAULT_MAXBINDCOUNT "256"
	// default max bind variable count

#define DEFAULT_MAXBINDNAMELENGTH "64"
	// default max bind variable length

#define DEFAULT_MAXSTRINGBINDVALUELENGTH "32768"
	// default max string bind value length

#define DEFAULT_MAXLOBBINDVALUELENGTH "71680"
	// default lob bind value length

#define DEFAULT_MAXERRORLENGTH "2048"
	// default error size

#define DEFAULT_IDLECLIENTTIMEOUT "-1"
	// default idle client timeout

#define DEFAULT_CONNECTIONID "defaultid"
	// default id for an individual set of connections

#define DEFAULT_CONNECTSTRING ""
	// default connect string

#define DEFAULT_METRIC "1"
	// default metric

#define DEFAULT_BEHINDLOADBALANCER "no"
	// default behind-load-balancer flag

#define DEFAULT_ROUTER_HOST ""
	// default router host

#define DEFAULT_ROUTER_PORT "0"
	// default router port

#define DEFAULT_ROUTER_SOCKET ""
	// default router socket

#define DEFAULT_ROUTER_USER ""
	// default router user

#define DEFAULT_ROUTER_PASSWORD ""
	// default router password

#define DEFAULT_ROUTER_PATTERN ""
	// default router pattern

#define DEFAULT_MAXLISTENERS "-1"
	// default maximum number of listeners

#define DEFAULT_LISTENERTIMEOUT "0"
	// default listener timeout

#define DEFAULT_RELOGINATSTART "no"
	// default re-login at start attribute

#define DEFAULT_TIMEQUERIESSEC "-1"
#define DEFAULT_TIMEQUERIESUSEC "-1"
	// default time queries attributes

#define	DEFAULT_FAKEINPUTBINDVARIABLES "no"
	// default fake input bind variables attribute

#define	DEFAULT_TRANSLATEBINDVARIABLES "no"
	// default translate bind variables attribute

#define DEFAULT_INTERVAL 30
	// default interval that the cachemanager will scan on
