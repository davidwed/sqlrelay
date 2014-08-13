// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <rudiments/signalclasses.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/error.h>
#include <rudiments/stdio.h>
#include <cmdline.h>
#include <datatypes.h>
#include <defines.h>
#include <config.h>


class status : public sqlrcontroller_svr {
	public:
		status();
		shmdata 		*getStatistics();
		uint32_t		getConnectionCount();
		uint32_t		getConnectedClientCount();
		bool			init(int argc, const char **argv);
		semaphoreset		*getSemset();
	private:
		bool	attachToSharedMemoryAndSemaphores(const char *tmpdir,
								const char *id);

		bool	connected;

		tempdir		*tmpdir;

		semaphoreset	*statussemset;

		shmdata		privateshm;
};

status::status() : sqlrcontroller_svr() {
	connected=false;
}

semaphoreset *status::getSemset() {
	return statussemset;
}

shmdata *status::getStatistics() {
	if (!statussemset) {
		return NULL;
	}
	statussemset->waitWithUndo(9);
	privateshm=*shm;
	statussemset->signalWithUndo(9);
	return &privateshm;
}

uint32_t status::getConnectionCount() {
	if (!shm) {
		return 0;
	}
	return shm->totalconnections;
}

uint32_t status::getConnectedClientCount() {
	if (!shm) {
		return 0;
	}
	return shm->connectedclients;
}

bool status::init(int argc, const char **argv) {
	
	cmdl=new cmdline(argc,argv);

	cfgfl=new sqlrconfigfile();
	tmpdir=new tempdir(cmdl);
	shm=NULL;

	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId())) {
		return false;
	}

	if (!attachToSharedMemoryAndSemaphores(tmpdir->getString(),
							cmdl->getId())) {
		return false;
	}

	shm=(shmdata *)idmemory->getPointer();
	if (!shm) {
		stderror.printf("failed to get pointer to shmdata\n");
		return false;
	}

	return true;
}

bool status::attachToSharedMemoryAndSemaphores(const char *tmpdir,
							const char *id) {
	
	size_t  idfilenamelen=charstring::length(tmpdir)+5+
		charstring::length(id)+1;
	char	*idfilename=new char[idfilenamelen];
	charstring::printf(idfilename,idfilenamelen,"%s/ipc/%s",tmpdir,id);

	key_t	key=file::generateKey(idfilename,1);

	idmemory=new sharedmemory();
	if (!idmemory->attach(key,sizeof(shmdata))) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to shared memory segment: ");
		stderror.printf("%s\n",err);
		delete[] err;
		delete idmemory;
		statussemset=NULL;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}

	statussemset=new semaphoreset();
	if (!statussemset->attach(key,11)) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to semaphore set: ");
		stderror.printf("%s\n",err);
		delete[] err;
		delete statussemset;
		delete idmemory;
		statussemset=NULL;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}

	delete[] idfilename;

	return true;
}

void printAcquisitionStatus(int32_t sem) {
	stdoutput.printf("%s (%d)\n",(sem)?"acquired    ":"not acquired",sem);
}

void printTriggeredStatus(int32_t sem) {
	stdoutput.printf("%s (%d)\n",(sem)?"triggered    ":"not triggered",sem);
}

int main(int argc, const char **argv) {

	#include <version.h>

	status	s;

	// open the connection
	// this will fail, just ignore it for now
	s.init(argc,argv);
	
	shmdata	*statistics=s.getStatistics();
	if (!statistics) {
		process::exit(0);
	}

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
	
	stdoutput.printf(
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
