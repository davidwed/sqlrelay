// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <rudiments/error.h>

#include <statusconnection.h>

#include <datatypes.h>

#include <config.h>

#include <stdlib.h>

#include <defines.h>

// the only real function in this class
// all the other functions have been turned into stubs
sqlrstatistics *statusconnection::getStatistics() {
	return statistics;
}

bool statusconnection::init(int argc, const char **argv) {

	shmdata		*shm;
	
	cmdl=new cmdline(argc,argv);

	// get the connection id from the command line
	connectionid=cmdl->value("-connectionid");
	if (!connectionid[0]) {
		connectionid=DEFAULT_CONNECTIONID;
		fprintf(stderr,"Warning: using default connectionid.\n");
	}

	cfgfl=new sqlrconfigfile();
//	authc=new authenticator(cfgfl);
	tmpdir=new tempdir(cmdl);

	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId(),
					getNumberOfConnectStringVars())) {
		return false;
	}

//	setUserAndGroup();

	#ifdef SERVER_DEBUG
	debugfile::openDebugFile("connection",cmdl->getLocalStateDir());
	#endif

	// handle the unix socket directory
//	if (cfgfl->getListenOnUnix()) {
//		setUnixSocketDirectory();
//	}

	// handle the pid file
//	if (!handlePidFile()) {
//		return false;
//	}
/*
	constr=cfgfl->getConnectString(connectionid);
	if (!constr) {
		fprintf(stderr,"Error: invalid connectionid \"%s\".\n",
							connectionid);
		return false;
	}
	handleConnectString();

	initDatabaseAvailableFileName();

	if (cfgfl->getListenOnUnix() &&
		!getUnixSocket(tmpdir->getString(),unixsocketptr)) {
		return false;
	}

	#ifndef SERVER_DEBUG
	bool	nodetach=cmdl->found("-nodetach");

	if (!nodetach && detachbeforeloggingin) {
		// detach from the controlling tty
		detach();
	}
	#endif
*/
//	blockSignals();

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

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"attaching to shared memory");
	debugPrint("connection",0,"id filename: ");
	debugPrint("connection",0,idfilename);
	#endif

	idmemory=new sharedmemory();
	if (!idmemory->attach(file::generateKey(idfilename,1))) {
		fprintf(stderr,"Couldn't attach to shared memory segment: ");
		fprintf(stderr,"%s\n",error::getErrorString());
		delete idmemory;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,
			"done attaching to shared memory and semaphores");
	#endif

	delete[] idfilename;

	return true;
}



		
statusconnection::statusconnection() : sqlrconnection_svr() {
	connected=false;
}


uint16_t statusconnection::getNumberOfConnectStringVars() {
	return 0;
}

bool statusconnection::supportsNativeBinds() {
	return false;
}

void statusconnection::handleConnectString() {
	
}

bool statusconnection::logIn() {
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

