// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#ifndef SQLRSHMDATA_H
#define SQLRSHMDATA_H

// FIXME: this is only here so the headers don't have to include defines.h
#define USERSIZE 128

// sizes...
#define MAXCONNECTIONIDLEN 256
#define MAXUNIXSOCKETLEN 1024
#define MAXCONNECTIONS 512
#define STATQPSKEEP 900
#define STATSQLTEXTLEN 512
#define STATCLIENTINFOLEN 512

// structures...
enum sqlrconnectionstate_t {
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
};

#endif
