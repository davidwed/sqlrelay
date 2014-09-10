// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#ifndef DEFINES_H
#define DEFINES_H

// listener-connection protocol...
#define HANDOFF_PASS 0
#define HANDOFF_RECONNECT 1
#define HANDOFF_PROXY 2

// client-server protocol...
#define SQLRCLIENT_PROTOCOL 0x5352 // 0x5352 = SR
#define NEW_QUERY 0
#define FETCH_RESULT_SET 1
#define ABORT_RESULT_SET 2
#define SUSPEND_RESULT_SET 3
#define RESUME_RESULT_SET 4
#define SUSPEND_SESSION 5
#define END_SESSION 6
#define PING 7
#define IDENTIFY 8
#define COMMIT 9
#define ROLLBACK 10
#define AUTHENTICATE 11
#define AUTOCOMMIT 12
#define REEXECUTE_QUERY 13
#define FETCH_FROM_BIND_CURSOR 14
#define DBVERSION 15
#define BINDFORMAT 16
#define SERVERVERSION 17
#define GETDBLIST 18
#define GETTABLELIST 19
#define GETCOLUMNLIST 20
#define SELECT_DATABASE 21
#define GET_CURRENT_DATABASE 22
#define GET_LAST_INSERT_ID 23
#define BEGIN 24
#define GET_QUERY_TREE 25
#define NO_COMMAND 26
#define DBHOSTNAME 27
#define DBIPADDRESS 28

#define SUSPENDED_RESULT_SET 1
#define NO_SUSPENDED_RESULT_SET 0

#define SEND_COLUMN_INFO 1
#define DONT_SEND_COLUMN_INFO 0

#define ERROR_OCCURRED 0
#define NO_ERROR_OCCURRED 1
#define ERROR_OCCURRED_DISCONNECT 2
#define TIMEOUT_GETTING_ERROR_STATUS 3

#define DONT_RECONNECT 0
#define RECONNECT 1

#define NEED_NEW_CURSOR 0
#define DONT_NEED_NEW_CURSOR 1

#define END_COLUMN_INFO 0
#define END_RESULT_SET 3

#define ACTUAL_ROWS 1
#define NO_ACTUAL_ROWS 0
#define AFFECTED_ROWS 1
#define NO_AFFECTED_ROWS 0

#define SKIP_ROWS 1
#define SKIP_NO_ROWS 0
#define FETCH_ROWS 1
#define FETCH_NO_ROWS 0

#define NULL_DATA 0
#define STRING_DATA 1
#define START_LONG_DATA 2
#define END_LONG_DATA 3
#define CURSOR_DATA 4
#define INTEGER_DATA 5
#define DOUBLE_DATA 6
#define DATE_DATA 7

#define END_BIND_VARS 8 

#define DONT_RE_EXECUTE 0
#define RE_EXECUTE 1

#define NULL_BIND 0
#define STRING_BIND 1
#define INTEGER_BIND 2
#define DOUBLE_BIND 3
#define BLOB_BIND 4
#define CLOB_BIND 5
#define CURSOR_BIND 6
#define DATE_BIND 7

// sizes...
// FIXME: these are duplicated here and in sqlrshmdata.h
#define USERSIZE 128
#define MAXCONNECTIONIDLEN 256
#define MAXUNIXSOCKETLEN 1024
#define MAXCONNECTIONS 512
/*#define STATQPSKEEP 900
#define STATSQLTEXTLEN 512
#define STATCLIENTINFOLEN 512*/

// errors...
// (hopefully the 900000+ range doesn't collide with anyone's native codes)
#define SQLR_ERROR_NOCURSORS 900000
#define SQLR_ERROR_NOCURSORS_STRING \
	"No server-side cursors were available to process the query."
#define SQLR_ERROR_MAXCLIENTINFOLENGTH 900001
#define SQLR_ERROR_MAXCLIENTINFOLENGTH_STRING \
	"Maximum client info length exceeded."
#define SQLR_ERROR_MAXQUERYLENGTH 900002
#define SQLR_ERROR_MAXQUERYLENGTH_STRING \
	"Maximum query length exceeded."
#define SQLR_ERROR_MAXBINDCOUNT 900003
#define SQLR_ERROR_MAXBINDCOUNT_STRING \
	"Maximum bind variable count exceeded."
#define SQLR_ERROR_MAXBINDNAMELENGTH 900004
#define SQLR_ERROR_MAXBINDNAMELENGTH_STRING \
	"Maximum bind variable name length exceeded."
#define SQLR_ERROR_MAXSTRINGBINDVALUELENGTH 900005
#define SQLR_ERROR_MAXSTRINGBINDVALUELENGTH_STRING \
	"Maximum string bind value length exceeded."
#define SQLR_ERROR_MAXLOBBINDVALUELENGTH 900006
#define SQLR_ERROR_MAXLOBBINDVALUELENGTH_STRING \
	"Maximum lob bind value length exceeded."
#define SQLR_ERROR_DUPLICATEBINDNAME 900007
#define SQLR_ERROR_DUPLICATEBINDNAME_STRING \
	"Duplicate bind variable name."
#define SQLR_ERROR_MAXSELECTLIST 900008
#define SQLR_ERROR_MAXSELECTLIST_STRING \
	"Maximum column count exceeded."
#define SQLR_ERROR_RESULTSETNOTSUSPENDED 900009
#define SQLR_ERROR_RESULTSETNOTSUSPENDED_STRING \
	"The requested result set was not suspended."
#define SQLR_ERROR_TOOMANYLISTENERS 900010
#define SQLR_ERROR_TOOMANYLISTENERS_STRING \
	"Too many listeners."
#define SQLR_ERROR_ERRORFORKINGLISTENER 900011
#define SQLR_ERROR_ERRORFORKINGLISTENER_STRING \
	"Error forking listener."
#define SQLR_ERROR_AUTHENTICATIONERROR 900012
#define SQLR_ERROR_AUTHENTICATIONERROR_STRING \
	"Authentication Error."
#define SQLR_ERROR_HANDOFFFAILED 900013
#define SQLR_ERROR_HANDOFFFAILED_STRING \
	"The listener failed to hand the client off to the database connection."
#define SQLR_ERROR_DBSDOWN 900014
#define SQLR_ERROR_DBSDOWN_STRING \
	"All databases are currently down."
#define SQLR_ERROR_LASTINSERTIDNOTSUPPORTED 900015
#define SQLR_ERROR_LASTINSERTIDNOTSUPPORTED_STRING \
	"Get last insert id not supported."
#define SQLR_ERROR_INVALIDBINDVARIABLEFORMAT 900016
#define SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING \
	"Invalid bind variable format."

enum clientsessiontype_t {
	SQLRELAY_CLIENT_SESSION_TYPE=0,
	MYSQL_CLIENT_SESSION_TYPE
};

// structures...
/*enum sqlrconnectionstate_t {
	NOT_AVAILABLE=0,
	INIT,
	WAIT_FOR_AVAIL_DB,
	WAIT_CLIENT,
	SESSION_START,
	GET_COMMAND,
	PROCESS_SQL,
	PROCESS_CUSTOM,
	RETURN_RESULT_SET,
	SESSION_END,
	ANNOUNCE_AVAILABILITY,
	WAIT_SEMAPHORE
};

struct sqlrconnstatistics {
	uint32_t			processid;
	enum sqlrconnectionstate_t	state;
	uint32_t			index;
	uint32_t			nconnect;
	uint32_t			nauthenticate;
	uint32_t			nsuspend_session;
	uint32_t			nend_session;
	uint32_t			nping;
	uint32_t			nidentify;
	uint32_t			nautocommit;
	uint32_t			nbegin;
	uint32_t			ncommit;
	uint32_t			nrollback;;
	uint32_t			ndbversion;
	uint32_t			nbindformat;
	uint32_t			nserverversion;
	uint32_t			nselectdatabase;
	uint32_t			ngetcurrentdatabase;
	uint32_t			ngetlastinsertid;
	uint32_t			ndbhostname;
	uint32_t			ndbipaddress;
	uint64_t			nnewquery;
	uint64_t			nreexecutequery;
	uint32_t			nfetchfrombindcursor;
	uint32_t			nfetchresultset;
	uint32_t			nabortresultset;
	uint32_t			nsuspendresultset;
	uint32_t			nresumeresultset;
	uint32_t			ngetdblist;
	uint32_t			ngettablelist;
	uint32_t			ngetcolumnlist;
	uint32_t			ngetquerytree;
	uint64_t			nsql;
	uint32_t			ncustomsql;
	uint32_t			nrelogin;
	uint64_t			loggedinsec;
	uint64_t			loggedinusec;
	uint64_t			statestartsec;
	uint64_t			statestartusec;
	uint64_t			clientsessionsec;
	uint64_t			clientsessionusec;
	char				clientaddr[16];
	char				clientinfo[STATCLIENTINFOLEN];
	char				sqltext[STATSQLTEXTLEN];
};

// This structure is used to pass data in shared memory between the listener
// and connection daemons.  A struct is used instead of just stepping a pointer
// through the shared memory segment to avoid alignment issues.
struct shmdata {

	uint32_t	totalconnections;
	char		connectionid[MAXCONNECTIONIDLEN];
	union {
		struct {
			uint16_t	inetport;
			char		unixsocket[MAXUNIXSOCKETLEN];
		} sockets;
		pid_t	connectionpid;
	} connectioninfo;

	uint32_t	connectedclients;

	time_t		starttime;

	uint32_t	open_db_connections;
	uint32_t	opened_db_connections;

	uint32_t	open_db_cursors;
	uint32_t	opened_db_cursors;

	uint32_t	open_cli_connections;
	uint32_t	opened_cli_connections;

	uint32_t	times_new_cursor_used;
	uint32_t	times_cursor_reused;

	uint32_t	total_queries;
	uint32_t	total_errors;

	uint32_t	forked_listeners;

	// below were added by neowiz...

	// maximum number of listeners allowed and
	// number of times that limit was was hit
	uint32_t	max_listeners;
	uint32_t	max_listeners_errors;

	// highest count of listeners
	// (all-time and over previous minute)
	uint32_t	peak_listeners;
	uint32_t	peak_listeners_1min;
	time_t		peak_listeners_1min_time;

	// highest count of connections-in-use
	// (all-time and over previous minute)
	uint32_t	peak_connectedclients;
	uint32_t	peak_connectedclients_1min;
	time_t		peak_connectedclients_1min_time;

	time_t		timestamp[STATQPSKEEP];
	uint32_t	qps_select[STATQPSKEEP];
	uint32_t	qps_insert[STATQPSKEEP];
	uint32_t	qps_update[STATQPSKEEP];
	uint32_t	qps_delete[STATQPSKEEP];
	uint32_t	qps_create[STATQPSKEEP];
	uint32_t	qps_drop[STATQPSKEEP];
	uint32_t	qps_alter[STATQPSKEEP];
	uint32_t	qps_custom[STATQPSKEEP];
	uint32_t	qps_etc[STATQPSKEEP];

	sqlrconnstatistics	connstats[MAXCONNECTIONS];
};*/

#endif
