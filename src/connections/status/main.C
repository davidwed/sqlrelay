// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <statusconnection.h>
#include <rudiments/signalclasses.h>
#include <defines.h>

#include <stdlib.h>
#include <stdio.h>

// for _exit
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

statusconnection	*conn;
signalhandler	*alarmhandler;

void cleanUp() {
	delete conn;
	delete alarmhandler;
}

void shutDown() {
	cleanUp();
	_exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	conn=new statusconnection();
	
	sqlrstatistics      *statistics;

	// open the connection
	// this will fail, just ignore it for now
	conn->init(argc,argv);
	
	statistics=conn->getStatistics();

	printf( 
		"  Open   Server Connections:  %d\n" 
		"  Opened Server Connections:  %d\n" 
		"\n"
		"  Open   Client Connections:  %d\n"
		"  Opened Client Connections:  %d\n"
		"\n"
		"  Open   Server Cursors:      %d\n"
		"  Opened Server Cursors:      %d\n"
		"\n"
		"  Times  New Cursor Used:     %d\n"
		"  Times  Cursor Reused:       %d\n"
		"\n"
		"  Total  Queries:             %d\n" 
		"  Total  Errors:              %d\n"
		"\n"
		"  Forked Listeners:           %d\n"
		"\n",
		statistics->open_svr_connections, 
		statistics->opened_svr_connections,
		statistics->open_cli_connections, 
		statistics->opened_cli_connections,
		statistics->open_svr_cursors,
		statistics->opened_svr_cursors,
		statistics->times_new_cursor_used,
		statistics->times_cursor_reused,
		statistics->total_queries,
		statistics->total_errors,
		statistics->forked_listeners
		);
	
	printf(
		"Scaler's view:\n"
		"  Connections:                %d\n"
		"  Sessions:                   %d\n"
		"\n",
		conn->getConnectionCount(),
		conn->getSessionCount()
		);

	#define SEM_COUNT	11
	int	sem[SEM_COUNT];
	for (int i=0; i<SEM_COUNT; i++) {
		sem[i]=conn->getSemset()->getValue(i);
	}

	printf(
		"Semaphores:\n"
		"  +---------------------------------------------+\n"
		"  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |  10 |\n"
		"  +---+---+---+---+---+---+---+---+---+---+-----+\n"
		"  | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %3d |\n"
		"  +---------------------------------------------+\n"
		"\n",
		sem[0],sem[1],sem[2],sem[3],sem[4],sem[5],sem[6],sem[7],sem[8],sem[9],sem[10]
		);


	cleanUp();
	exit(1);
}
