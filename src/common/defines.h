// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#ifndef DEFINES_H
#define DEFINES_H

#define SUSPENDED_RESULT_SET 1
#define NO_SUSPENDED_RESULT_SET 0

#define SEND_COLUMN_INFO 1
#define DONT_SEND_COLUMN_INFO 0

#define NEW_QUERY 0
#define REEXECUTE_QUERY 13
#define FETCH_FROM_BIND_CURSOR 14
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

#define ERROR 0
#define NO_ERROR 1

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
#define NORMAL_DATA 1
#define START_LONG_DATA 2
#define END_LONG_DATA 3
#define CURSOR_DATA 4

#define END_BIND_VARS 5 

#define DONT_RE_EXECUTE 0
#define RE_EXECUTE 1

#ifndef MAXPATHLEN
	#define MAXPATHLEN 256
#endif

#define USERSIZE 128

#define BINDVARLENGTH 30
#define MAXVAR 256

#define MAXCONNECTIONIDLEN 1024

// This structure is used to pass data in shared memory between the listener
// and connection daemons.  A struct is used instead of just stepping a pointer
// through the shared memory segment to avoid alignment issues.
struct shmdata {
	uint32_t	totalconnections;
	uint32_t	connectionsinuse;
	char		connectionid[MAXCONNECTIONIDLEN];
	union {
		struct {
			uint16_t	inetport;
			char		unixsocket[MAXPATHLEN];
		} sockets;
		pid_t	connectionpid;
	} connectioninfo;
};

#endif
