// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <rudiments/error.h>

#include <statusconnection.h>

#include <datatypes.h>

#include <config.h>

#include <stdlib.h>

#include <defines.h>

sqlrstatistics *statusconnection::getStatistics() {
	statussemset->waitWithUndo(9);
	privatestatistics=*statistics;
	statussemset->signalWithUndo(9);
	return &privatestatistics;
}

int statusconnection::getConnectionCount() {
	return ((shmdata *)idmemory->getPointer())->totalconnections;
}

int statusconnection::getSessionCount() {
	return ((shmdata *)idmemory->getPointer())->connectionsinuse;
}

bool statusconnection::init(int argc, const char **argv) {

	shmdata		*shm;
	
	cmdl=new cmdline(argc,argv);

	// get the connection id from the command line
	connectionid=cmdl->getValue("-connectionid");
	if (!connectionid[0]) {
		connectionid=DEFAULT_CONNECTIONID;
		fprintf(stderr,"Warning: using default connectionid.\n");
	}

	cfgfl=new sqlrconfigfile();
	tmpdir=new tempdir(cmdl);

	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId(),
					getNumberOfConnectStringVars())) {
		return false;
	}

	dbgfile.init("connection",cmdl->getLocalStateDir());
	if (cmdl->found("-debug")) {
		dbgfile.enable();
	}

	if (!createSharedMemoryAndSemaphores(tmpdir->getString(),
							cmdl->getId())) {
		return false;
	}

	shm=(shmdata *)idmemory->getPointer();
	if (!shm) {
		fprintf(stderr,"failed to get pointer to shmdata\n");
		return false;
	}

	statistics=&shm->statistics;
	if (!statistics) {
		fprintf(stderr,"failed to point statistics at idmemory\n");
	}

	return true;
}

bool statusconnection::createSharedMemoryAndSemaphores(const char *tmpdir,
		const char *id) {
	
	size_t  idfilenamelen=charstring::length(tmpdir)+5+
		charstring::length(id)+1;
	char	*idfilename=new char[idfilenamelen];
	snprintf(idfilename,idfilenamelen,"%s/ipc/%s",tmpdir,id);

	key_t	key=file::generateKey(idfilename,1);

	dbgfile.debugPrint("connection",0,"attaching to shared memory and semaphores");
	dbgfile.debugPrint("connection",0,"id filename: ");
	dbgfile.debugPrint("connection",0,idfilename);
	idmemory=new sharedmemory();
	if (!idmemory->attach(key)) {
		fprintf(stderr,"Couldn't attach to shared memory segment: ");
		fprintf(stderr,"%s\n",error::getErrorString());
		delete idmemory;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}

	statussemset=new semaphoreset();
	if (!statussemset->attach(key,11)) {
		fprintf(stderr,"Couldn't attach to semaphore set: ");
		fprintf(stderr,"%s\n",error::getErrorString());
		delete statussemset;
		delete idmemory;
		statussemset=NULL;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}

	dbgfile.debugPrint("connection",0,
			"done attaching to shared memory and semaphores");

	delete[] idfilename;

	return true;
}

statusconnection::statusconnection() : sqlrconnection_svr() {
	connected=false;
}

uint16_t statusconnection::getNumberOfConnectStringVars() {
	return 0;
}

void statusconnection::handleConnectString() {
}

bool statusconnection::logIn(bool printerrors) {
	return false;
}

sqlrcursor_svr *statusconnection::initCursor() {
	return (sqlrcursor_svr *)false;
}

void statusconnection::deleteCursor(sqlrcursor_svr *curs) {
}

void statusconnection::logOut() {
}

const char *statusconnection::identify() {
	return "status";
}

const char *statusconnection::dbVersion() {
	return "";
}

bool statusconnection::isTransactional() {
	return false;
}

bool statusconnection::autoCommitOn() {
	return false;
}

bool statusconnection::autoCommitOff() {
	return false;
}

bool statusconnection::commit() {
	return false;
}

bool statusconnection::rollback() {
	return false;
}

semaphoreset *statusconnection::getSemset() {
	return statussemset;
}
