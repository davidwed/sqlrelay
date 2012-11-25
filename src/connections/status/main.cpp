// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <statusconnection.h>
#include <rudiments/signalclasses.h>
#include <rudiments/process.h>
#include <defines.h>

#include <stdlib.h>
#include <stdio.h>

void printAcquisitionStatus(int32_t sem) {
	printf("%s (%d)\n",(sem)?"acquired    ":"not acquired",sem);
}

void printTriggeredStatus(int32_t sem) {
	printf("%s (%d)\n",(sem)?"triggered    ":"not triggered",sem);
}

int main(int argc, const char **argv) {

	#include <version.h>

	status	s;

	// open the connection
	// this will fail, just ignore it for now
	s.init(argc,argv);
	
	shmdata	*statistics=s.getStatistics();

	printf( 
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
		"\n",
		statistics->open_db_connections, 
		statistics->opened_db_connections,
		statistics->open_db_cursors,
		statistics->opened_db_cursors,
		statistics->open_cli_connections, 
		statistics->opened_cli_connections,
		statistics->times_new_cursor_used,
		statistics->times_cursor_reused,
		statistics->total_queries,
		statistics->total_errors,
		statistics->forked_listeners
		);
	
	printf(
		"Scaler's view:\n"
		"  Connections:                  %d\n"
		"  Connected Clients:            %d\n"
		"\n",
		s.getConnectionCount(),
		s.getConnectedClientCount()
		);

	#define SEM_COUNT	11
	int32_t	sem[SEM_COUNT];
	for (uint16_t i=0; i<SEM_COUNT; i++) {
		sem[i]=s.getSemset()->getValue(i);
	}

	printf("Mutexes:\n");
	printf("  Connection Announce               : ");
	printAcquisitionStatus(sem[0]);
	printf("  Shared Memory Access              : ");
	printAcquisitionStatus(sem[1]);
	printf("  Connection Count                  : ");
	printAcquisitionStatus(sem[4]);
	printf("  Session Count                     : ");
	printAcquisitionStatus(sem[5]);
	printf("  Open Connections/Forked Listeners : ");
	printAcquisitionStatus(sem[9]);
	printf("\n");

	printf("Triggers:\n");
	printf("  Accept Available Connection (l-w, c-s)         : ");
	printTriggeredStatus(sem[2]);
	printf("  Done Accepting Available Connection (c-w, l-s) : ");
	printTriggeredStatus(sem[3]);
	printf("  Evaluate Connection Count (s-w, l-s)           : ");
	printTriggeredStatus(sem[6]);
	printf("  Done Evaluating Connection Count (l-w, s-s)    : ");
	printTriggeredStatus(sem[7]);
	printf("  Connection Has Started (s-w, c-s)              : ");
	printTriggeredStatus(sem[8]);
	printf("\n");

	printf("Counts:\n");
	printf("  Busy Listener Count : %d\n",sem[10]);

	printf("\n");

	printf("Raw Semaphores:\n"
		"  +---------------------------------------------+\n"
		"  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |  10 |\n"
		"  +---+---+---+---+---+---+---+---+---+---+-----+\n"
		"  | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %3d |\n"
		"  +---------------------------------------------+\n",
		sem[0],sem[1],sem[2],sem[3],sem[4],
		sem[5],sem[6],sem[7],sem[8],sem[9],sem[10]
		);


	process::exit(0);
}
