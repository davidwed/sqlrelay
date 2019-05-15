// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <rudiments/commandline.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/error.h>
#include <rudiments/stdio.h>
#include <sqlrelay/private/sqlrshm.h>
#include <sqlrelay/sqlrutil.h>
#include <datatypes.h>
#include <defines.h>
#include <config.h>
#include <version.h>

static void printAcquisitionStatus(int32_t sem) {
	stdoutput.printf("%s (%d)\n",(sem)?"acquired    ":"not acquired",sem);
}

static void printTriggeredStatus(int32_t sem) {
	stdoutput.printf("%s (%d)\n",(sem)?"triggered    ":"not triggered",sem);
}

static const char *sqlrconnectionstateStr(sqlrconnectionstate_t s) {
	// FIXME: use an array instead, and move it to sqlrelay.h?
	if (s == NOT_AVAILABLE) {
		return "NOT_AVAILABLE";
	}
	if (s == INIT) {
		return "INIT";
	}
	if (s == WAIT_FOR_AVAIL_DB) {
		return "WAIT_FOR_AVAIL_DB";
	}
	if (s == WAIT_CLIENT) {
		return "WAIT_CLIENT";
	}
	if (s == SESSION_START) {
		return "SESSION_START";
	}
	if (s == GET_COMMAND) {
		return "GET_COMMAND";
	}
	if (s == PROCESS_SQL) {
		return "PROCESS_SQL";
	}
	if (s == PROCESS_CUSTOM) {
		return "PROCESS_CUSTOM";
	}
	if (s == RETURN_RESULT_SET) {
		return "RETURN_RESULT_SET";
	}
	if (s == SESSION_END) {
		return "SESSION_END";
	}
	if (s == ANNOUNCE_AVAILABILITY) {
		return "ANNOUNCE_AVAILABILITY";
	}
	if (s == WAIT_SEMAPHORE) {
		return "WAIT_SEMAPHORE";
	}
	return "undefined";
}

static void printQuery(sqlrconnstatistics *conn) {
	// We could use stdoutput.safePrint().  But to take up as little space
	// as possible we want to compress whitespace and just ignore non-ascii
	// characters, which is not the right thing, but in practice is ok in
	// many environments.
	stdoutput.printf("\n");
	bool prev_space=true;
	bool prev_newline=true;
	size_t	k;
	for (k=0; k<sizeof(conn->sqltext); k++) {
		int c=conn->sqltext[k];
		if (c==0) {
			break;
		} else if ((c=='\t') || (c==' ')) {
			if (!prev_space) {
				stdoutput.write((char) ' ');
				prev_space=true;
			}
			prev_newline=false;
		} else if ((c>' ') && (c<128)) {
			prev_space=false;
			prev_newline=false;
			stdoutput.write((char) c);
		} else if (c=='\n') {
			if (!prev_newline) {
				stdoutput.write((char) c);
				prev_newline=true;
			}
			prev_space=true;
		}
	}
	if (k>0) {
		if (!prev_newline) {
			stdoutput.printf("\n");
		}
		stdoutput.printf("\n");
	}
}

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s status utility.\n"
		"\n"
		"The %s utility examines the specified instance and reports its current state, various counts, and some statistical information.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		SERVEROPTIONS
		"\n"
		"Examples:\n"
		"\n"
		"Check the status of the specified instance, as defined in the default\n"
		"configuration.\n"
		"\n"
		"	%s -id myinst\n"
		"\n"
		"Check the status of the specified instance, as defined in the config file\n"
		"./myconfig.conf\n"
		"\n"
		"	%s -config ./myconfig.conf -id myinst\n"
		"\n",
		progname,SQL_RELAY,progname,progname,progname,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	// parse the command line
	sqlrcmdline	cmdl(argc,argv);

	const char	*id=cmdl.getValue("-id");
	if (charstring::isNullOrEmpty(id)) {
		stdoutput.printf("usage:\n"
			" %s-status [-config config] -id id "
			"[-localstatedir dir] [-short] "
			"[-connection-detail [-query]]\n",SQLR);
		process::exit(1);
	}
	bool		shortoutput=cmdl.found("-short");
	bool		connoutput=cmdl.found("-connection-detail");
	bool		queryoutput=cmdl.found("-query");
	
	// get the id filename and key
	sqlrpaths	sqlrp(&cmdl);
	stringbuffer	idfilename;
	idfilename.append(sqlrp.getIpcDir())->append(id)->append(".ipc");
	key_t	key=file::generateKey(idfilename.getString(),1);

	// attach to the shared memory segment for the specified instance
	sharedmemory	idmemory;
	if (!idmemory.attach(key,sizeof(sqlrshm))) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to shared memory segment: ");
		stderror.printf("%s\n",err);
		delete[] err;
		process::exit(0);
	}
	sqlrshm	*shm=(sqlrshm *)idmemory.getPointer();
	if (!shm) {
		stderror.printf("failed to get pointer to shm\n");
		process::exit(0);
	}

	// attach to the semaphore set for the specified instance
	semaphoreset	semset;
	if (!semset.attach(key,13)) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to semaphore set: ");
		stderror.printf("%s\n",err);
		delete[] err;
		process::exit(0);
	}

	// take a snapshot of the stats
	semset.waitWithUndo(9);
	sqlrshm		statistics=*shm;
	semset.signalWithUndo(9);
	#define SEM_COUNT	13
	int32_t	sem[SEM_COUNT];
	for (uint16_t i=0; i<SEM_COUNT; i++) {
		sem[i]=semset.getValue(i);
	}

	if (shortoutput) {
		// print out key=value pairs on one line for easier reading by
		// other programs such as zabbix
		stdoutput.printf("enabled=%d "
				"open_database_connections=%d "
				"opened_database_connections=%d "
				"open_database_cursors=%d "
				"opened_database_cursors=%d "
				"open_client_connections=%d "
				"opened_client_connections=%d "
				"new_cursor_used=%d "
				"cursor_reused=%d "
				"total_queries=%d "
				"total_errors=%d\n",
				!statistics.disabled,
				statistics.open_db_connections,
				statistics.opened_db_connections,
				statistics.open_db_cursors,
				statistics.opened_db_cursors,
				statistics.open_cli_connections,
				statistics.opened_cli_connections,
				statistics.times_new_cursor_used,
				statistics.times_cursor_reused,
				statistics.total_queries,
				statistics.total_errors);
		process::exit(0);
	}

	// print out stats
	stdoutput.printf( 
		"  Instance State:               %s\n"
		"\n"
		"  Open   Database Connections:  %d\n" 
		"  Opened Database Connections:  %d\n" 
		"\n"
		"  Open   Database Cursors:      %d\n"
		"  Opened Database Cursors:      %d\n"
		"\n"
		"  Open   Client Connections:    %d\n"
		"  Opened Client Connections:    %d\n"
		"\n"
		"  Times  New Cursor Used:       %d\n"
		"  Times  Cursor Reused:         %d\n"
		"\n"
		"  Total  Queries:               %d\n" 
		"  Total  Errors:                %d\n"
		"\n"
		"  Forked Listeners:             %d\n"
		"\n"
		"Scaler's view:\n"
		"  Connections:                  %d\n"
		"  Connected Clients:            %d\n"
		"\n",
		(statistics.disabled)?"Disabled":"Enabled",
		statistics.open_db_connections, 
		statistics.opened_db_connections,
		statistics.open_db_cursors,
		statistics.opened_db_cursors,
		statistics.open_cli_connections, 
		statistics.opened_cli_connections,
		statistics.times_new_cursor_used,
		statistics.times_cursor_reused,
		statistics.total_queries,
		statistics.total_errors,
		statistics.forked_listeners,
		statistics.totalconnections,
		statistics.connectedclients
		);

	stdoutput.printf("Mutexes:\n");
	stdoutput.printf("  Connection Announce               : ");
	printAcquisitionStatus(sem[0]);
	stdoutput.printf("  Shared Memory Access              : ");
	printAcquisitionStatus(sem[1]);
	stdoutput.printf("  Connection Count                  : ");
	printAcquisitionStatus(sem[4]);
	stdoutput.printf("  Session Count                     : ");
	printAcquisitionStatus(sem[5]);
	stdoutput.printf("  Open Connections/Forked Listeners : ");
	printAcquisitionStatus(sem[9]);
	stdoutput.printf("\n");

	stdoutput.printf("Triggers:\n");
	stdoutput.printf("  Accept Available Connection (l-w, c-s)         : ");
	printTriggeredStatus(sem[2]);
	stdoutput.printf("  Done Accepting Available Connection (c-w, l-s) : ");
	printTriggeredStatus(sem[3]);
	stdoutput.printf("  Connection Ready For Handoff (l-w, c-s)        : ");
	printTriggeredStatus(sem[12]);
	stdoutput.printf("  Evaluate Connection Count (s-w, l-s)           : ");
	printTriggeredStatus(sem[6]);
	stdoutput.printf("  Done Evaluating Connection Count (l-w, s-s)    : ");
	printTriggeredStatus(sem[7]);
	stdoutput.printf("  Connection Has Started (s-w, c-s)              : ");
	printTriggeredStatus(sem[8]);
	stdoutput.printf("\n");

	stdoutput.printf("Counts:\n");
	stdoutput.printf("  Busy Listener Count : %d\n",sem[10]);

	stdoutput.printf("\n");

	stdoutput.printf("Raw Semaphores:\n"
		"  +-------------------------------------------------------+\n"
		"  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |  10 | 11 | 12 |\n"
		"  +---+---+---+---+---+---+---+---+---+---+-----+----+----+\n"
		"  | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %3d | %2d | %2d |\n"
		"  +-------------------------------------------------------+\n",
		sem[0],sem[1],sem[2],sem[3],sem[4],
		sem[5],sem[6],sem[7],sem[8],sem[9],
		sem[10],sem[11],sem[12]
		);

	if (connoutput) {
		long conndim=sizeof(statistics.connstats)/
				sizeof(struct sqlrconnstatistics);
		sqlrconnstatistics *conn=&statistics.connstats[0];
		stdoutput.printf("\n");
		stdoutput.printf("Info for max=%ld connections id=%s\n\n",
					conndim,&statistics.connectionid[0]);
		for(long j=0; j<conndim; j++) {
			if (conn[j].state!=NOT_AVAILABLE) {
				// print out multiple lines, a cross between
				// ease of human readability and potentially
				// automated parsing.
				stdoutput.printf("%ld: "
						"pid=%d "
						"state=%s (%d) "
						"nconnect=%d "
						"nauth=%d "
						"nsuspend=%d "
						"nend=%d "
						"nrelogin=%d "
						"loggedinsec=%d "
						"statestartsec=%d "
						"clientsessionsec=%d\n",
						j,conn[j].processid,
						sqlrconnectionstateStr(
								conn[j].state),
						conn[j].state,
						conn[j].nconnect,
						conn[j].nauth,
						conn[j].nsuspend_session,
						conn[j].nend_session,
						conn[j].nrelogin,
						conn[j].loggedinsec,
						conn[j].statestartsec,
						conn[j].clientsessionsec);
				// elsewhere in the code the strings
				// are treated as zero terminated.
				stdoutput.printf(" clientinfo=%s "
						"clientaddr=%s "
						"user=%s\n",
						&conn[j].clientinfo[0],
						&conn[j].clientaddr[0],
						&conn[j].user[0]);
				stdoutput.printf(" nautocommit=%d "
						"nbegin=%d "
						"ncommit=%d "
						"nrollback=%d "
						"ndbversion=%d "
						"nbindformat=%d "
						"nserverversion=%d "
						"nselectdatabase=%d\n",
						conn[j].nautocommit,
						conn[j].nbegin,
						conn[j].ncommit,
						conn[j].nrollback,
						conn[j].ndbversion,
						conn[j].nbindformat,
						conn[j].nserverversion,
						conn[j].nselectdatabase);
				stdoutput.printf(" ngetcurrentdatabase=%d "
						"ngetlastinsertid=%d "
						"ngettablelist=%d "
						"ngetcolumnlist=%d "
						"ngetquerytree=%d\n",
						conn[j].ngetcurrentdatabase,
						conn[j].ngetlastinsertid,
						conn[j].ngettablelist,
						conn[j].ngetcolumnlist,
						conn[j].ngetquerytree);
				stdoutput.printf(" ndbhostname=%d "
						"ndbipaddress=%d "
						"nfetchfrombindcursor=%d "
						"nfetchresultset=%d "
						"nabortresultset=%d "
						"nsuspendresultset=%d "
						"nresumeresultset=%d "
						"ngetdblist=%d\n",
						conn[j].ndbhostname,
						conn[j].ndbipaddress,
						conn[j].nfetchfrombindcursor,
						conn[j].nfetchresultset,
						conn[j].nabortresultset,
						conn[j].nsuspendresultset,
						conn[j].nresumeresultset,
						conn[j].ngetdblist);
				stdoutput.printf(" nping=%d "
						"nidentify=%d "
						"nnewquery=%d "
						"nreexecutequery=%d "
						"nsql=%d "
						"ncustomsql=%d "
						"nnextresultset=%d "
						"nnextresultsetavailable=%d\n",
						conn[j].nping,
						conn[j].nidentify,
						conn[j].nnewquery,
						conn[j].nreexecutequery,
						conn[j].nsql,
						conn[j].ncustomsql,
						conn[j].nnextresultset,
						conn[j].nnextresultsetavailable
						);
				if (queryoutput) {
					printQuery(&(conn[j]));
				}
			}
		}
	}

	process::exit(0);
}
