// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <statusconnection.h>
#include <rudiments/signalclasses.h>
#include <defines.h>

#include <stdlib.h>
#include <stdio.h>

statusconnection	*conn;
signalhandler	*alarmhandler;

void cleanUp() {
//	conn->closeConnection();
	delete conn;
	delete alarmhandler;
}

void shutDown() {
	cleanUp();
	exit(0);
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
		"  Total  Queries:      %d\n" 
		"  Total  Errors:       %d\n"
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
		statistics->total_errors
		);
	

	// unsuccessful completion
	cleanUp();
	exit(1);
}
