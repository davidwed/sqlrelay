// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#ifndef DEFINES_H
#define DEFINES_H

// listener-connection protocol
#define HANDOFF_PASS 0
#define HANDOFF_RECONNECT 1

// client-server protocol
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

#define SUSPENDED_RESULT_SET 1
#define NO_SUSPENDED_RESULT_SET 0

#define SEND_COLUMN_INFO 1
#define DONT_SEND_COLUMN_INFO 0

#define ERROR_OCCURRED 0
#define NO_ERROR_OCCURRED 1
#define ERROR_OCCURRED_DISCONNECT 2

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

// sizes
#ifndef MAXPATHLEN
	#define MAXPATHLEN 256
#endif

#define USERSIZE 128

#define MAXCONNECTIONIDLEN 1024

// errors
// (hopefully the 900000+ range doesn't collide with anyone's native codes)
#define SQLR_ERROR_NO_CURSORS 900000
#define SQLR_ERROR_NO_CURSORS_STRING \
	"No server-side cursors were available to process the query."
#define SQLR_ERROR_MAXSELECTLIST 900001
#define SQLR_ERROR_MAXSELECTLIST_STRING \
	"Maximum column count exceeded."
#define SQLR_ERROR_MAXBINDCOUNT 900002
#define SQLR_ERROR_MAXBINDCOUNT_STRING \
	"Bind variable count exceeded."
#define SQLR_ERROR_MAXBINDNAMELENGTH 900003
#define SQLR_ERROR_MAXBINDNAMELENGTH_STRING \
	"Bind variable name length exceeded."
#define SQLR_ERROR_DUPLICATE_BINDNAME 900004
#define SQLR_ERROR_DUPLICATE_BINDNAME_STRING \
	"Duplicate bind variable name."
#define SQLR_ERROR_VARIABLE_NOT_BOUND 900005
#define SQLR_ERROR_VARIABLE_NOT_BOUND_STRING \
	"Variable present but not bound."
#define SQLR_ERROR_MAXSTRINGBINDLEN 900006
#define SQLR_ERROR_MAXSTRINGBINDLEN_STRING \
	"String bind variable length exceeded."

// structures
struct sqlrstatistics {
	int32_t	open_svr_connections;
	int32_t	opened_svr_connections;

	int32_t	open_cli_connections;
	int32_t	opened_cli_connections;

	int32_t	open_svr_cursors;
	int32_t	opened_svr_cursors;

	int32_t	times_new_cursor_used;
	int32_t	times_cursor_reused;

	int32_t	total_queries;
	int32_t	total_errors;

	int32_t	forked_listeners;
};

// This structure is used to pass data in shared memory between the listener
// and connection daemons.  A struct is used instead of just stepping a pointer
// through the shared memory segment to avoid alignment issues.
struct shmdata {
	int32_t		totalconnections;
	int32_t		connectionsinuse;
	char		connectionid[MAXCONNECTIONIDLEN];
	union {
		struct {
			uint16_t	inetport;
			char		unixsocket[MAXPATHLEN];
		} sockets;
		pid_t	connectionpid;
	} connectioninfo;
	sqlrstatistics	statistics;
};

enum clientsessiontype_t {
	SQLRELAY_CLIENT_SESSION_TYPE=0,
	MYSQL_CLIENT_SESSION_TYPE
};

#endif
