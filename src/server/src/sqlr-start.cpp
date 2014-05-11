// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#include <sqlrconfigfile.h>
#include <cmdline.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

// for ceil()
#include <math.h>

#define MAX_CONNECTIONS 200

int32_t getConnections(sqlrconfigfile *cfgfile, bool override) {
	int32_t	connections=cfgfile->getConnections();
	if (!override && connections>MAX_CONNECTIONS) {
		connections=MAX_CONNECTIONS;
	}
	return connections;
}

bool startListener(const char *id, const char *config,
					const char *localstatedir) {

	// start the listener
	stdoutput.printf("\nStarting listener:\n");

	// build command to spawn
	const char	*cmd="sqlr-listener";
	uint16_t	i=0;
	const char	*args[8];
	args[i++]="sqlr-listener";
	args[i++]="-id";
	args[i++]=id;
	args[i++]="-config";
	args[i++]=config;
	if (charstring::length(localstatedir)) {
		args[i++]="-localstatedir";
		args[i++]=localstatedir;
	}
	args[i]=NULL;
	
	// display command
	stdoutput.printf("  ");
	for (uint16_t index=0; index<i; index++) {
		stdoutput.printf("%s ",args[index]);
	}
	stdoutput.printf("\n");

	// spawn the command
	if (process::spawn(cmd,args)==-1) {
		stdoutput.printf("\nsqlr-listener failed to start.\n");
		return false;
	}
	return true;
}


bool startConnection(bool strace, const char *id, const char *connectionid,
				const char *config, const char *localstatedir) {

	// build command to spawn
	const char	*cmd=NULL;
	uint16_t	i=0;
	const char	*args[14];
	if (strace) {
		cmd="strace";
		args[i++]="strace";
		args[i++]="-ff";
		args[i++]="-o";
	} else {
		cmd="sqlr-connection";
	}
	args[i++]="sqlr-connection";
	args[i++]="-id";
	args[i++]=id;
	if (connectionid) {
		args[i++]="-connectionid";
		args[i++]=connectionid;
	}
	args[i++]="-config";
	args[i++]=config;
	if (charstring::length(localstatedir)) {
		args[i++]="-localstatedir";
		args[i++]=localstatedir;
	}
	if (strace) {
		args[i++]="&";
	}
	args[i]=NULL;

	// display command
	stdoutput.printf("  ");
	for (uint16_t index=0; index<i; index++) {
		stdoutput.printf("%s ",args[index]);
	}
	stdoutput.printf("\n");

	// spawn the command
	if (process::spawn(cmd,args)==-1) {
		stdoutput.printf("\nsqlr-connection failed to start.\n");
		return false;
	}
	return true;
}

bool startConnections(sqlrconfigfile *cfgfile, bool strace,
				const char *id, const char *config,
				const char *localstatedir,
				bool overridemaxconn) {

	// get the connection count and total metric
	linkedlist< connectstringcontainer *>	*connectionlist=
						cfgfile->getConnectStringList();

	// if the metrictotal was 0, start no connections
	if (!cfgfile->getMetricTotal()) {
		return true;
	}

	// if no connections were defined in the config file,
	// start 1 default one
	if (!cfgfile->getConnectionCount()) {
		return !startConnection(strace,id,config,localstatedir,NULL);
	}

	// get number of connections
	int32_t	connections=getConnections(cfgfile,overridemaxconn);

	// start the connections
	connectstringnode	*csn=connectionlist->getFirstNode();
	connectstringcontainer	*csc;
	int32_t	metric=0;
	int32_t	startup=0;
	int32_t	totalstarted=0;
	bool	done=false;
	while (!done) {

		// get the appropriate connection
		csc=csn->getValue();
	
		// scale the number of each connection to start by 
		// each connection's metric vs the total number of 
		// connections to start up
		metric=csc->getMetric();
		if (metric>0) {
			startup=(int32_t)ceil(
				((double)(metric*connections))/
				((double)cfgfile->getMetricTotal()));
		} else {
			startup=0;
		}

		// keep from starting too many connections
		if (totalstarted+startup>connections) {
			startup=connections-totalstarted;
			done=true;
		}

		stdoutput.printf("\nStarting %d connections to ",startup);
		stdoutput.printf("%s :\n",csc->getConnectionId());

		// fire them up
		for (int32_t i=0; i<startup; i++) {
			if (!startConnection(strace,id,
						csc->getConnectionId(),
						config,localstatedir)) {
				// it's ok if at least 1 connection started up
				return (totalstarted>0 || i>0);
			}
		}

		// have we started enough connections?
		totalstarted=totalstarted+startup;
		if (totalstarted==connections) {
			done=true;
		}

		// next...
		csn=csn->getNext();
	}
	return true;
}

bool startScaler(sqlrconfigfile *cfgfile, const char *id,
			const char *config, const char *localstatedir) {

	// don't start the scalar if unless dynamic scaling is enabled
	if (!cfgfile->getDynamicScaling()) {
		return true;
	}

	stdoutput.printf("\nStarting scaler:\n");

	// build command to spawn
	const char	*cmd="sqlr-scaler";
	uint16_t	i=0;
	const char	*args[8];
	args[i++]="sqlr-scaler";
	args[i++]="-id";
	args[i++]=id;
	args[i++]="-config";
	args[i++]=config;
	if (charstring::length(localstatedir)) {
		args[i++]="-localstatedir";
		args[i++]=localstatedir;
	}
	args[i]=NULL;

	// display command
	stdoutput.printf("  ");
	for (uint16_t index=0; index<i; index++) {
		stdoutput.printf("%s ",args[index]);
	}
	stdoutput.printf("\n");

	// spawn the command
	if (process::spawn(cmd,args)==-1) {
		stdoutput.printf("\nsqlr-scaler failed to start.\n");
		return false;
	}
	return true;
}

int main(int argc, const char **argv) {

	#include <version.h>

	// get the command line args
	cmdline	cmdl(argc,argv);
	const char	*localstatedir=cmdl.getValue("-localstatedir");
	bool		strace=cmdl.found("-strace");
	const char	*id=cmdl.getId();
	const char	*config=cmdl.getConfig();
	bool		overridemaxconn=cmdl.found("-overridemaxconnections");

	// default id warning
	if (!charstring::compare(cmdl.getId(),DEFAULT_ID)) {
		stderror.printf("Warning: using default id.\n");
	}

	// parse the config file(s)
	sqlrconfigfile	cfgfile;
	if (!cfgfile.parse(config,id)) {
		process::exit(1);
	}

	// start listener, connections, scaler, cachemanager
	bool	exitstatus=!(startListener(id,config,localstatedir) &&
			startConnections(&cfgfile,strace,id,config,
					localstatedir,overridemaxconn) &&
			startScaler(&cfgfile,id,config,localstatedir));

	// many thanks...
	// these companies don't exist any more so it's
	// probably ok not to display the attribution any more
	/*stdoutput.printf("\n\nThanks to MP3.com for sponsoring: \n");
	stdoutput.printf("	Clustered/Replicated database support.\n");
	stdoutput.printf("	Perl API.\n");
	stdoutput.printf("Thanks to FeedLounge for sponsoring: \n");
	stdoutput.printf("	Query routing and filtering.\n");*/
	
	// successful exit
	process::exit(exitstatus);
}
