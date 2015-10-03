// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/error.h>
#include <rudiments/stdio.h>
#include <sqlrelay/private/sqlrshmdata.h>
#include <sqlrelay/sqlrutil.h>
#include <datatypes.h>
#include <defines.h>
#include <config.h>


void printAcquisitionStatus(int32_t sem) {
	stdoutput.printf("%s (%d)\n",(sem)?"acquired    ":"not acquired",sem);
}

void printTriggeredStatus(int32_t sem) {
	stdoutput.printf("%s (%d)\n",(sem)?"triggered    ":"not triggered",sem);
}

int main(int argc, const char **argv) {

	#include <version.h>

	// parse the command line
	sqlrcmdline	cmdl(argc,argv);
	sqlrpaths	sqlrpth(&cmdl);
	sqlrconfig	cfg(&sqlrpth);

	// parse the config file
	if (!cfg.parse(sqlrpth.getConfigFile(),cmdl.getId())) {
		process::exit(0);
	}
	
	// get the id filename and key
	sqlrpaths	sqlrp(&cmdl);
	stringbuffer	idfilename;
	idfilename.append(sqlrp.getIpcDir())->append("/")->append(cmdl.getId());
	key_t	key=file::generateKey(idfilename.getString(),1);

	// attach to the shared memory segment for the specified instance
	sharedmemory	idmemory;
	if (!idmemory.attach(key,sizeof(shmdata))) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to shared memory segment: ");
		stderror.printf("%s\n",err);
		delete[] err;
		process::exit(0);
	}
	shmdata	*shm=(shmdata *)idmemory.getPointer();
	if (!shm) {
		stderror.printf("failed to get pointer to shmdata\n");
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
	shmdata		statistics=*shm;
	semset.signalWithUndo(9);
	#define SEM_COUNT	13
	int32_t	sem[SEM_COUNT];
	for (uint16_t i=0; i<SEM_COUNT; i++) {
		sem[i]=semset.getValue(i);
	}

	// print out stats
	stdoutput.printf( 
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

	process::exit(0);
}
