// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#include <rudiments/commandline.h>
#include <scaler.h>

#include <rudiments/permissions.h>
#include <rudiments/file.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

scaler::scaler() : daemonprocess() {

	tmpdirlen=strlen(TMP_DIR);
	init=false;

	pidfile=NULL;
	semset=NULL;
	idmemory=NULL;
	cfgfile=NULL;

	id=NULL;
	config=NULL;
	dbase=NULL;

	debug=0;
}

scaler::~scaler() {
	if (init) {
		cleanUp();
	}
}

bool scaler::initScaler(int argc, const char **argv) {

	init=true;

	// read the commandline
	commandline	cmdl(argc,argv);

	// get the id
	char	*tmpid=cmdl.value("-id");
	if (!(tmpid && tmpid[0])) {
		tmpid=DEFAULT_ID;
		fprintf(stderr,"Warning! using default id.\n");
	}
	id=strdup(tmpid);

	// check for listener's pid file
	char	*listenerpidfile=new char[tmpdirlen+15+strlen(id)+1];
	sprintf(listenerpidfile,"%s/sqlr-listener-%s",TMP_DIR,id);
	if (checkForPidFile(listenerpidfile)==-1) {
		fprintf(stderr,"\nsqlr-scaler error: \n");
		fprintf(stderr,"	The file %s",TMP_DIR);
		fprintf(stderr,"/sqlr-listener-%s",id);
		fprintf(stderr," was not found.\n");
		fprintf(stderr,"	This usually means that the ");
		fprintf(stderr,"sqlr-listener is not running.\n");
		fprintf(stderr,"	The sqlr-listener must be running ");
		fprintf(stderr,"for the sqlr-scaler to start.\n\n");
		delete[] listenerpidfile;
		return false;
	}
	delete[] listenerpidfile;

	// check/set pid file
	pidfile=new char[tmpdirlen+13+strlen(id)+1];
	sprintf(pidfile,"%s/sqlr-scaler-%s",TMP_DIR,id);
	if (checkForPidFile(pidfile)!=-1) {
		fprintf(stderr,"\nsqlr-scaler error:\n");
		fprintf(stderr,"	The pid file %s",TMP_DIR);
		fprintf(stderr,"/sqlr-scaler-%s",id);
		fprintf(stderr," exists.\n");
		fprintf(stderr,"	This usually means that the ");
		fprintf(stderr,"sqlr-scaler is already running for the \n");
		fprintf(stderr,"	%s instance.\n",id);
		fprintf(stderr,"	If it is not running, please remove ");
		fprintf(stderr,"the file and restart.\n");
		delete[] pidfile;
		pidfile=NULL;
		return false;
	}

	// check for debug
	if (cmdl.found("-debug")) {
		debug=1;
	}

	// get the config file
	char	*tmpconfig=cmdl.value("-config");
	if (!(tmpconfig && tmpconfig[0])) {
		tmpconfig=DEFAULT_CONFIG_FILE;
	}
	config=new char[strlen(tmpconfig)+1];
	strcpy(config,tmpconfig);

	// parse the config file
	cfgfile=new sqlrconfigfile;
	if (cfgfile->parse(config,id)) {

		// don't even start if we're not using dynamic scaling
		if (!cfgfile->getDynamicScaling()) {
			return false;
		}

		// run as user/group specified in the config file
		char	*runasuser=cfgfile->getRunAsUser();
		char	*runasgroup=cfgfile->getRunAsGroup();
		if (runasuser[0] && runasgroup[0]) {
			runAsUser(runasuser);
			runAsGroup(runasgroup);
		}

		// get the dynamic connection scaling parameters
		maxconnections=cfgfile->getMaxConnections();
		maxqueuelength=cfgfile->getMaxQueueLength();
		growby=cfgfile->getGrowBy();
		ttl=cfgfile->getTtl();

		// get the database type
		dbase=strdup(cfgfile->getDbase());

		// get the list of connect strings
		connectstringlist=cfgfile->getConnectStringList();

		// add up the connection metrics
		metrictotal=cfgfile->getMetricTotal();
	}

	// create the pid file
	createPidFile(pidfile,permissions::ownerReadWrite());

	// initialize the shared memory segment filename
	idfilename=new char[tmpdirlen+1+strlen(id)+1];
	sprintf(idfilename,"%s/%s",TMP_DIR,id);
	key_t	key=ftok(idfilename,0);

	// connect to the semaphore set
	semset=new semaphoreset;
	semset->attach(key,11);

	// connect to the shared memory segment
	idmemory=new sharedmemory;
	idmemory->attach(key);

	// set up random number generator
	currentseed=time(NULL);

	// detach from the controlling tty
	detach();

	return true;
}

void scaler::cleanUp() {

	delete[] idfilename;

	delete semset;
	delete idmemory;
	delete cfgfile;
	delete[] id;

	if (pidfile) {
		unlink(pidfile);
		delete[] pidfile;
	}

	delete[] config;
	delete[] dbase;
}


void scaler::openMoreConnections() {

	// wait for a new listener to fire up and increment the listener count
	semset->wait(6);

	int	sessions=countSessions();
	int	connections=countConnections();

	// signal listener to keep going
	semset->signal(7);

	// do we need to open more connections?
	if ((sessions-connections)<=maxqueuelength) {
		return;
	}

	// can more be opened, or will we exceed the max?
	if ((connections+growby)<=maxconnections) {

		// open "growby" connections
		for (int i=0; i<growby; i++) {

			// loop until a connection is successfully started
			bool	success=false;
			while (!success) {

				getRandomConnectionId();

				// if the database associated with the
				// connection id that was randomly chosen is
				// currently unavailable, loop back and get
				// another one
				if (!availableDatabase()) {
					sleep(1);
					continue;
				}

				// build the command to start a connection
				char	*command=new char[16+strlen(dbase)+
							6+20+
							5+strlen(id)+
							15+strlen(connectionid)+
							9+strlen(config)+
							2+1];
				sprintf(command,
	"sqlr-connection-%s%s -ttl %d -id %s -connectionid %s -config %s %s",
					dbase,((debug)?"-debug":""),
					ttl,id,connectionid,config,
					((debug)?"&":""));

				// start another connection
				success=!system(command);

				// clean up
				delete[] command;

				// wait for the connection count to increase
				if (success) {
					semset->wait(8);
				}
			}
		}
	}
}

void scaler::getRandomConnectionId() {

	// get a scaled random number
	currentseed=randomnumber::generateNumber(currentseed);
	int	scalednum=randomnumber::scaleNumber(currentseed,0,metrictotal);

	// run through list, decrementing scalednum by the metric
	// for each, when scalednum is 0, pick that connection id
	connectstringnode	*csn=connectstringlist->getNodeByIndex(0);
	while (csn) {
		connectstringcontainer	*currentnode=csn->getData();
		scalednum=scalednum-currentnode->getMetric();
		if (scalednum<=0) {
			connectionid=currentnode->getConnectionId();
			break;
		}
		csn=csn->getNext();
	}
}

bool scaler::availableDatabase() {
	
	// initialize the database up/down filename
	char	*updown=new char[tmpdirlen+1+strlen(id)+1+
					strlen(connectionid)+1];
	sprintf(updown,"%s/%s-%s",TMP_DIR,id,connectionid);
	bool	retval=file::exists(updown);
	delete[] updown;
	return retval;
}

int scaler::countSessions() {

	// get the number of open connections
	unsigned int	*sessions=(unsigned int *)((long)idmemory->getPointer()+
							sizeof(unsigned int));

	return (int)(*sessions);
}

int scaler::countConnections() {

	// wait for access to the connection counter
	semset->wait(4);

	// get the number of connections
	unsigned int	*connections=(unsigned int *)idmemory->getPointer();

	// signal that the connection counter may be accessed by someone else
	semset->signal(4);

	return (int)(*connections);
}

void scaler::loop() {

	for (;;) {
		// open more connections if necessary
		openMoreConnections();
	}
}
