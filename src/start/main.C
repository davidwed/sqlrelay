// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#include <sqlrconfigfile.h>
#include <cmdline.h>
#include <rudiments/sleep.h>

#include <math.h>
#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

int getConnections(sqlrconfigfile *cfgfile) {
	int	connections=cfgfile->getConnections();
	if (connections>MAX_CONNECTIONS) {
		connections=MAX_CONNECTIONS;
	}
	return connections;
}

bool startListener(const char *id, const char *config,
			const char *localstatedir, int listenerdebug) {

	// start the listener
	printf("\nStarting listener:\n");
	
	stringbuffer	command;
	command.append("sqlr-listener");
	if (listenerdebug) {
		command.append("-debug");
	}
	command.append(" -id ")->append(id);
	command.append(" -config ")->append(config);
	if (localstatedir[0]) {
		command.append(" -localstatedir ")->append(localstatedir);
	}
	if (listenerdebug) {
		command.append(" &");
	}
	printf("  %s\n",command.getString());

	bool	success=!system(command.getString());

	if (!success) {
		printf("\nsqlr-listener failed to start.\n");
	}

	sleep::macrosleep(1);

	return success;
}


bool startConnection(int strace, const char *dbase,
				const char *id, const char *connectionid,
				const char *config, const char *localstatedir,
				int connectiondebug) {

	stringbuffer	command;
	if (strace) {
		command.append("strace -ff -o sqlr-connection-strace ");
	}
	command.append("sqlr-connection-")->append(dbase);
	if (connectiondebug) {
		command.append("-debug");
	}
	command.append(" -id ")->append(id);
	if (connectionid) {
		command.append(" -connectionid ")->append(connectionid);
	}
	command.append(" -config ")->append(config);
	if (localstatedir[0]) {
		command.append(" -localstatedir ")->append(localstatedir);
	}
	if (strace || connectiondebug) {
		command.append(" &");
	}

	printf("  %s\n",command.getString());

	bool	success=!system(command.getString());

	if (!success) {
		printf("\nsqlr-connection-%s failed to start.\n",dbase);
	}

	return success;
}

bool startConnections(sqlrconfigfile *cfgfile, int strace,
				const char *id, const char *config,
				const char *localstatedir,
				int connectiondebug) {

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
		return !startConnection(strace,cfgfile->getDbase(),id,config,
					localstatedir,NULL,connectiondebug);
	}

	// get number of connections
	int	connections=getConnections(cfgfile);

	// start the connections
	connectstringnode	*csn=connectionlist->getNodeByIndex(0);
	connectstringcontainer	*csc;
	int	metric=0;
	int	startup=0;
	int	totalstarted=0;
	bool	done=false;
	while (!done) {

		// get the appropriate connection
		csc=csn->getData();
	
		// scale the number of each connection to start by 
		// each connection's metric vs the total number of 
		// connections to start up
		metric=csc->getMetric();
		if (metric>0) {
			startup=(int)ceil(
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

		printf("\nStarting %d connections to ",startup);
		printf("%s :\n",csc->getConnectionId());

		// fire them up
		for (int i=0; i<startup; i++) {
			if (!startConnection(strace,cfgfile->getDbase(),
					id,csc->getConnectionId(),
					config,localstatedir,connectiondebug)) {
				return false;
			}
			sleep::macrosleep(1);
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

bool startScaler(sqlrconfigfile *cfgfile, const char *id, const char *config,
			const char *localstatedir, int connectiondebug) {

	// don't start the scalar if unless dynamic scaling is enabled
	if (!cfgfile->getDynamicScaling()) {
		return true;
	}

	printf("\nStarting scaler:\n");
	
	stringbuffer	command;
	command.append("sqlr-scaler ")->append(" -id ")->append(id);
	if (connectiondebug) {
		command.append(" -debug ");
	}
	command.append(" -config ")->append(config);
	if (localstatedir[0]) {
		command.append(" -localstatedir ")->append(localstatedir);
	}
	printf("  %s\n",command.getString());

	bool	success=!system(command.getString());

	if (!success) {
		printf("\nsqlr-scaler failed to start.\n");
	}

	return success;
}

bool startCacheManager(const char *localstatedir) {

	// create a ps command that will detect if the cachemanager is running
	stringbuffer	command;
	command.append(PS)->append(" | grep sqlr-cachemanager | grep -v grep");
	
	// run the command
	FILE	*cmd=popen(command.getString(),"r");
	command.clear();
	
	// get the result
	stringbuffer	contents;
	char		character;
	while (!feof(cmd) && (character=fgetc(cmd))>-1) {
		contents.append(character);
	}
	pclose(cmd);
	
	// if the cachemanger isn't running, start it
	if (!charstring::length(contents.getString())) {
	
		printf("\nStarting cache manager:\n");
	
		command.append("sqlr-cachemanager");
		if (localstatedir[0]) {
			command.append(" -cachedirs ")->append(localstatedir);
			command.append("/sqlrelay/cache");
		}
		printf("  %s\n",command.getString());

		bool	success=!system(command.getString());

		if (!success) {
			printf("\nsqlr-cachemanager failed to start.\n");
			return false;
		}
	
	} else {
		printf("\ncache manager already running.\n");
	}

	return true;
}

int main(int argc, const char **argv) {

	#include <version.h>

	// get the command line args
	cmdline	cmdl(argc,argv);
	const char	*localstatedir=cmdl.value("-localstatedir");
	int		strace=cmdl.found("-strace");
	const char	*id=cmdl.getId();
	const char	*config=cmdl.getConfig();

	// parse the config file(s)
	sqlrconfigfile	cfgfile;
	if (!cfgfile.parse(config,id)) {
		exit(1);
	}

	// start listener, connections, scaler, cachemanager
	int	exitstatus=!(startListener(id,config,
				localstatedir,cfgfile.getDebugListener()) &&
			startConnections(&cfgfile,strace,id,config,
				localstatedir,cfgfile.getDebugConnection()) &&
			startScaler(&cfgfile,id,config,localstatedir,
				cfgfile.getDebugConnection()) &&
			startCacheManager(localstatedir));

	// many thanks...
	printf("\n\nThanks to MP3.com for sponsoring: \n");
	printf("	Clustered/Replicated database support.\n");
	printf("	Perl API.\n");
	
	// successful exit
	exit(exitstatus);
}
