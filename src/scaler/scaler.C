// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#include <rudiments/commandline.h>
#include <rudiments/snooze.h>
#include <scaler.h>

#include <rudiments/permissions.h>
#include <rudiments/file.h>
#include <rudiments/passwdentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/datetime.h>

#include <defines.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

scaler::scaler() : daemonprocess() {

	init=false;

	cmdl=NULL;

	pidfile=NULL;
	semset=NULL;
	idmemory=NULL;
	cfgfile=NULL;
	tmpdir=NULL;

	id=NULL;
	config=NULL;
	dbase=NULL;

	debug=false;
}

scaler::~scaler() {
	if (init) {
		cleanUp();
	}
}

bool scaler::initScaler(int argc, const char **argv) {

	init=true;

	// read the commandline
	cmdl=new cmdline(argc,argv);

	// get the id
	const char	*tmpid=cmdl->getValue("-id");
	if (!(tmpid && tmpid[0])) {
		tmpid=DEFAULT_ID;
		fprintf(stderr,"Warning! using default id.\n");
	}
	id=charstring::duplicate(tmpid);

	tmpdir=new tempdir(cmdl);

	// check for listener's pid file
	size_t	listenerpidfilelen=tmpdir->getLength()+20+
					charstring::length(id)+1;
	char	*listenerpidfile=new char[listenerpidfilelen];
	snprintf(listenerpidfile,listenerpidfilelen,
			"%s/pids/sqlr-listener-%s",tmpdir->getString(),id);
	if (checkForPidFile(listenerpidfile)==-1) {
		fprintf(stderr,"\nsqlr-scaler error: \n");
		fprintf(stderr,"	The file %s",listenerpidfile);
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
	size_t	pidfilelen=tmpdir->getLength()+18+charstring::length(id)+1;
	pidfile=new char[pidfilelen];
	snprintf(pidfile,pidfilelen,
			"%s/pids/sqlr-scaler-%s",tmpdir->getString(),id);
	if (checkForPidFile(pidfile)!=-1) {
		fprintf(stderr,"\nsqlr-scaler error:\n");
		fprintf(stderr,"	The pid file %s",pidfile);
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
	debug=cmdl->found("-debug");

	// get the config file
	const char	*tmpconfig=cmdl->getValue("-config");
	if (!(tmpconfig && tmpconfig[0])) {
		tmpconfig=DEFAULT_CONFIG_FILE;
	}
	config=new char[charstring::length(tmpconfig)+1];
	charstring::copy(config,tmpconfig);

	// parse the config file
	cfgfile=new sqlrconfigfile;
	if (cfgfile->parse(config,id)) {

		// don't even start if we're not using dynamic scaling
		if (!cfgfile->getDynamicScaling()) {
			return false;
		}

		// run as user/group specified in the config file
		const char	*runasuser=cfgfile->getRunAsUser();
		const char	*runasgroup=cfgfile->getRunAsGroup();
		if (runasuser[0] && runasgroup[0]) {

			// get the user that we're currently running as
			char	*currentuser=NULL;
			passwdentry::getName(process::getEffectiveUserId(),
						&currentuser);

			// get the group that we're currently running as
			char	*currentgroup=NULL;
			groupentry::getName(process::getEffectiveGroupId(),
						&currentgroup);

			// switch groups, but only if we're not currently
			// running as the group that we should switch to
			if (charstring::compare(currentgroup,
						cfgfile->getRunAsGroup()) &&
					!runAsGroup(cfgfile->getRunAsGroup())) {
				fprintf(stderr,"Warning: could not change ");
				fprintf(stderr,"group to %s\n",
						cfgfile->getRunAsGroup());
			}

			// switch users, but only if we're not currently
			// running as the user that we should switch to
			if (charstring::compare(currentuser,
						cfgfile->getRunAsUser()) &&
					!runAsUser(cfgfile->getRunAsUser())) {
				fprintf(stderr,"Warning: could not change ");
				fprintf(stderr,"user to %s\n",
						cfgfile->getRunAsUser());
			}

			// clean up
			delete[] currentuser;
			delete[] currentgroup;
		}

		// make sure user/group can read the config file
		// (This shouldn't be necessary because if the user/group
		// can't read the file, the sqlr-listener won't start and if
		// it won't start, the scaler won't start.  However someone
		// could get crafty and force the sqlr-scaler to start so
		// we'll do this check just to make sure)
		file	test;
		if (!test.open(config,O_RDONLY)) {
			fprintf(stderr,"\nsqlr-scaler error:\n");
			fprintf(stderr,"	This instance of ");
			fprintf(stderr,"SQL Relay is ");
			fprintf(stderr,"configured to run as:\n");
			fprintf(stderr,"		user: %s\n",
						cfgfile->getRunAsUser());
			fprintf(stderr,"		group: %s\n\n",
						cfgfile->getRunAsGroup());
			fprintf(stderr,"	However, the config file %s\n",
								config);
			fprintf(stderr,"	cannot be read by that user ");
			fprintf(stderr,"or group.\n\n");
			fprintf(stderr,"	Since you're using ");
			fprintf(stderr,"dynamic scaling ");
			fprintf(stderr,"(ie. maxconnections>connections),\n");
			fprintf(stderr,"	new connections would be ");
			fprintf(stderr,"started as\n");
			fprintf(stderr,"		user: %s\n",
						cfgfile->getRunAsUser());
			fprintf(stderr,"		group: %s\n\n",
						cfgfile->getRunAsGroup());
			fprintf(stderr,"	They would not be able to ");
			fprintf(stderr,"read the");
			fprintf(stderr,"config file and would shut down.\n\n");
			fprintf(stderr,"	To remedy this problem, ");
			fprintf(stderr,"make %s\n",config);
			fprintf(stderr,"	readable by\n");
			fprintf(stderr,"		user: %s\n",
						cfgfile->getRunAsUser());
			fprintf(stderr,"		group: %s\n",
						cfgfile->getRunAsGroup());
			return false;
		}
		test.close();

		// get the dynamic connection scaling parameters
		maxconnections=cfgfile->getMaxConnections();
		maxqueuelength=cfgfile->getMaxQueueLength();
		growby=cfgfile->getGrowBy();
		ttl=cfgfile->getTtl();

		// get the database type
		dbase=charstring::duplicate(cfgfile->getDbase());

		// get the list of connect strings
		connectstringlist=cfgfile->getConnectStringList();

		// add up the connection metrics
		metrictotal=cfgfile->getMetricTotal();
	}

	// initialize the shared memory segment filename
	size_t	idfilenamelen=tmpdir->getLength()+5+charstring::length(id)+1;
	idfilename=new char[idfilenamelen];
	snprintf(idfilename,idfilenamelen,"%s/ipc/%s",tmpdir->getString(),id);
	key_t	key=file::generateKey(idfilename,1);

	// connect to the semaphore set
	semset=new semaphoreset;
	semset->attach(key,11);

	// connect to the shared memory segment
	idmemory=new sharedmemory;
	idmemory->attach(key);

	// set up random number generator
	datetime	dt;
	dt.getSystemDateAndTime();
	currentseed=dt.getEpoch();

	// detach from the controlling tty
	detach();

	// create the pid file
	createPidFile(pidfile,permissions::ownerReadWrite());

	return true;
}

void scaler::cleanUp() {

	delete[] idfilename;

	delete semset;
	delete idmemory;
	delete cfgfile;
	delete[] id;

	delete tmpdir;

	if (pidfile) {
		file::remove(pidfile);
		delete[] pidfile;
	}

	delete[] config;
	delete[] dbase;

	delete cmdl;
}


bool scaler::openMoreConnections() {

	// wait for a new listener to fire up and increment the listener count
	if (!semset->wait(6)) {
		// If this returns false then an error has occurred and the
		// semaphore can't be accessed.  Most likely the sqlr-listener
		// has been killed.
		return false;
	}

	int	sessions=countSessions();
	int	connections=countConnections();

	// signal listener to keep going
	semset->signal(7);

	// do we need to open more connections?
	if ((sessions-connections)<=maxqueuelength) {
		return true;
	}

	// can more be opened, or will we exceed the max?
	if ((connections+growby)<=maxconnections) {

		// open "growby" connections
		for (int32_t i=0; i<growby; i++) {

			// loop until a connection is successfully started
			bool	success=false;
			while (!success) {

				getRandomConnectionId();

				// if the database associated with the
				// connection id that was randomly chosen is
				// currently unavailable, loop back and get
				// another one
				// if no connections are currently open then
				// we won't know if the database is up or down
				// because no connections have tried to log
				// in to it yet, so in that case, don't even
				// test to see if the database is up or down
				if (connections && !availableDatabase()) {
					snooze::macrosnooze(1);
					continue;
				}

				// build the command to start a connection
				size_t	commandlen=16+charstring::length(dbase)+
						6+20+
						5+charstring::length(id)+
						15+charstring::length(
								connectionid)+
						9+charstring::length(config)+
						2+1;
				char	*command=new char[commandlen];
				snprintf(command,commandlen,
	"sqlr-connection-%s%s -silent -ttl %d -id %s -connectionid %s -config %s %s",
					dbase,((debug)?"-debug":""),
					ttl,id,connectionid,config,
					((debug)?"&":""));

				// start another connection
				success=!system(command);
				delete[] command;

				// wait for the connection count to increase
				if (success) {
					semset->wait(8);
				}
			}
		}
	}

	return true;
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
	size_t	updownlen=tmpdir->getLength()+5+charstring::length(id)+1+
					charstring::length(connectionid)+1;
	char	*updown=new char[updownlen];
	snprintf(updown,updownlen,"%s/ipc/%s-%s",
			tmpdir->getString(),id,connectionid);
	bool	retval=file::exists(updown);
	delete[] updown;
	return retval;
}

int32_t scaler::countSessions() {

	// get the number of open connections
	shmdata	*ptr=(shmdata *)idmemory->getPointer();
	return ptr->connectionsinuse;
}

int32_t scaler::countConnections() {

	// wait for access to the connection counter
	semset->waitWithUndo(4);

	// get the number of connections
	shmdata	*ptr=(shmdata *)idmemory->getPointer();
	uint32_t	totalconnections=ptr->totalconnections;

	// signal that the connection counter may be accessed by someone else
	semset->signalWithUndo(4);

	return totalconnections;
}

void scaler::loop() {
	while (openMoreConnections()) {}
}
