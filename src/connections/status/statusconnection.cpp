// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/error.h>

#include <statusconnection.h>

#include <cmdline.h>

#include <datatypes.h>

#include <config.h>

#include <stdlib.h>

#include <defines.h>

status::status() : sqlrcontroller_svr() {
	connected=false;
}

semaphoreset *status::getSemset() {
	return statussemset;
}

shmdata *status::getStatistics() {
	statussemset->waitWithUndo(9);
	rawbuffer::copy(&privateshm,shm,sizeof(shmdata));
	statussemset->signalWithUndo(9);
	return &privateshm;
}

int status::getConnectionCount() {
	return ((shmdata *)idmemory->getPointer())->totalconnections;
}

int status::getSessionCount() {
	return ((shmdata *)idmemory->getPointer())->connectionsinuse;
}

bool status::init(int argc, const char **argv) {

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

	if (!cfgfl->parse(cmdl->getConfig(),cmdl->getId())) {
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

	return true;
}

bool status::createSharedMemoryAndSemaphores(const char *tmpdir,
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
		char	*err=error::getErrorString();
		fprintf(stderr,"Couldn't attach to shared memory segment: ");
		fprintf(stderr,"%s\n",err);
		delete[] err;
		delete idmemory;
		idmemory=NULL;
		delete[] idfilename;
		return false;
	}

	statussemset=new semaphoreset();
	if (!statussemset->attach(key,11)) {
		char	*err=error::getErrorString();
		fprintf(stderr,"Couldn't attach to semaphore set: ");
		fprintf(stderr,"%s\n",err);
		delete[] err;
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
