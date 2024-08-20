// sqlr-connection/sqlr-listener parameters

// default id for listeners/connections
#define DEFAULT_ID "defaultid"

// default addresses for listeners/connections
#define DEFAULT_ADDRESS "0.0.0.0"

// default port to listen on
#define DEFAULT_PORT "9000"

// default client-server protocol
#define DEFAULT_PROTOCOL "sqlrclient"

// default database type
#define DEFAULT_DBASE "oracle"

// default number of connections to start
#define DEFAULT_CONNECTIONS "5"

// default action to take at end of session
#define DEFAULT_ENDOFSESSION "commit"

// default maximum queue length before
// another connection will be fired off
#define DEFAULT_MAXQUEUELENGTH "0"

// default number of connections to grow by
#define DEFAULT_GROWBY "1"

// default time to live for idle connections
// that were fired off to handle increased load
#define DEFAULT_TTL "60"

// default "retirement age" for connections
// that were fired off to handle increased load
#define DEFAULT_SOFTTTL "0"

// default max client sessions for connections
// that were fired off to handle increased load
#define DEFAULT_MAXSESSIONCOUNT "0"

// default session timeout
#define DEFAULT_SESSIONTIMEOUT "600"

// default number of cursors to open
#define DEFAULT_CURSORS "1"

// default maximum number of cursors to open
#define DEFAULT_MAXCURSORS "5"

// when we need more cursors, allocate this many more in one shot
#define DEFAULT_CURSORS_GROWBY "1"

// default tier to auth users on
#define DEFAULT_AUTHTIER "connection"

// default method to use for handing client sessions
#define DEFAULT_SESSION_HANDLER "thread"

// default method to use for handing off
// clients from listener to connection
#define DEFAULT_HANDOFF "pass"

// default regular expression for IP's that are allowed to connect
#define DEFAULT_ALLOWEDIPS ""

// default regular expression for IP's that are not allowed to connect
#define DEFAULT_DENIEDIPS ""

// default things to debug
#define DEFAULT_DEBUG "none"

// default parser debug
#define DEFAULT_DEBUG_PARSER false

// default directives
#define DEFAULT_DEBUG_DIRECTIVES false

// default query translations
#define DEFAULT_DEBUG_QUERYTRANSLATIONS false

// default filters
#define DEFAULT_DEBUG_FILTERS false

// default triggers
#define DEFAULT_DEBUG_TRIGGERS false

// default bind variable translations
#define DEFAULT_DEBUG_BINDVARIABLETRANSLATIONS false

// default result set translations
#define DEFAULT_DEBUG_RESULTSETTRANSLATIONS false

// default result set row translations
#define DEFAULT_DEBUG_RESULTSETROWTRANSLATIONS false

// default result set row block translations
#define DEFAULT_DEBUG_RESULTSETROWBLOCKTRANSLATIONS false

// default result set header translations
#define DEFAULT_DEBUG_RESULTSETHEADERTRANSLATIONS false

// default erorr translations
#define DEFAULT_DEBUG_ERRORTRANSLATIONS false

// default protocols
#define DEFAULT_DEBUG_PROTOCOLS false

// default auths
#define DEFAULT_DEBUG_AUTHS false

// default password encryptions
#define DEFAULT_DEBUG_PWDENCS false

// default loggers
#define DEFAULT_DEBUG_LOGGERS false

// default notifications
#define DEFAULT_DEBUG_NOTIFICATIONS false

// default schedules
#define DEFAULT_DEBUG_SCHEDULES false

// default routers
#define DEFAULT_DEBUG_ROUTERS false

// default queries
#define DEFAULT_DEBUG_QUERIES false

// default moduledatas
#define DEFAULT_DEBUG_MODULEDATAS false

// default max client info size
#define DEFAULT_MAXCLIENTINFOSIZE "512"

// default max query size
#define DEFAULT_MAXQUERYSIZE "65536"

// default max bind variable count
#define DEFAULT_MAXBINDCOUNT "256"

// default max bind variable size
#define DEFAULT_MAXBINDNAMESIZE "64"

// default max string bind value size
#define DEFAULT_MAXSTRINGBINDVALUESIZE "32768"

// default lob bind value size
#define DEFAULT_MAXLOBBINDVALUESIZE "71680"

// default error size
#define DEFAULT_MAXERRORSIZE "2048"

// default idle client timeout
#define DEFAULT_IDLECLIENTTIMEOUT "-1"

// default id for an individual set of connections
#define DEFAULT_CONNECTIONID "defaultid"

// default connect string
#define DEFAULT_CONNECTSTRING ""

// default metric
#define DEFAULT_METRIC "1"

// default behind-load-balancer flag
#define DEFAULT_BEHINDLOADBALANCER "no"

// default router host
#define DEFAULT_ROUTER_HOST ""

// default router port
#define DEFAULT_ROUTER_PORT "0"

// default router socket
#define DEFAULT_ROUTER_SOCKET ""

// default router user
#define DEFAULT_ROUTER_USER ""

// default router password
#define DEFAULT_ROUTER_PASSWORD ""

// default router pattern
#define DEFAULT_ROUTER_PATTERN ""

// default maximum number of listeners
#define DEFAULT_MAXLISTENERS "-1"

// default listener timeout
#define DEFAULT_LISTENERTIMEOUT "0"

// default re-login at start attribute
#define DEFAULT_RELOGINATSTART "no"

// default time queries attributes
#define DEFAULT_TIMEQUERIESSEC "-1"
#define DEFAULT_TIMEQUERIESUSEC "-1"

// default fake input bind variables attribute
#define	DEFAULT_FAKEINPUTBINDVARIABLES "no"

// default fake input bind variables unicode strings attribute
#define	DEFAULT_FAKEINPUTBINDVARIABLESUNICODESTRINGS "no"

// default translate bind variables attribute
#define	DEFAULT_TRANSLATEBINDVARIABLES "no"

// default bind variable delimiters
#define	DEFAULT_BINDVARIABLEDELIMITERS "?:@$"

// default interval that the cachemanager will scan on
#define DEFAULT_INTERVAL 30

// default kerberos service
#define DEFAULT_KRBSERVICE SQLRELAY

// default connection-start attempts
#define DEFAULT_CONNECTION_START_ATTEMPTS 5

// default connection-start timeout
#define DEFAULT_CONNECTION_START_TIMEOUT 20
