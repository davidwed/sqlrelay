// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#include <sqlrconfigfile.h>
#include <cmdline.h>

#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int	getConnections(sqlrconfigfile *cfgfile) {
	int	connections=cfgfile->getConnections();
	if (connections==0) {
		connections=1;
	}
	if (connections>MAX_CONNECTIONS) {
		connections=MAX_CONNECTIONS;
	}
	return connections;
}

int	startListener(const char *id, const char *config,
			const char *localstatedir, int listenerdebug) {

	// start the listener
	printf("\nStarting listener:\n");
	
	stringbuffer	*command=new stringbuffer();
	command->append("sqlr-listener");
	if (listenerdebug) {
		command->append("-debug");
	}
	command->append(" -id ")->append(id);
	command->append(" -config ")->append(config);
	if (localstatedir[0]) {
		command->append(" -localstatedir ")->append(localstatedir);
	}
	if (listenerdebug) {
		command->append(" &");
	}
	printf("  %s\n",command->getString());

	int	success=(system(command->getString())==0);
	delete command;

	sleep(1);

	return success;
}


int	startConnection(int strace, const char *dbase,
				const char *id, const char *connectionid,
				const char *config, const char *localstatedir,
				int connectiondebug) {

	stringbuffer	*command=new stringbuffer();
	if (strace) {
		command->append("strace -ff -o sqlr-connection-strace ");
	}
	command->append("sqlr-connection-")->append(dbase);
	if (connectiondebug) {
		command->append("-debug");
	}
	command->append(" -id ")->append(id);
	if (connectionid) {
		command->append(" -connectionid ")->append(connectionid);
	}
	command->append(" -config ")->append(config);
	if (localstatedir[0]) {
		command->append(" -localstatedir ")->append(localstatedir);
	}
	if (strace || connectiondebug) {
		command->append(" &");
	}

	printf("  %s\n",command->getString());

	int	success=(system(command->getString())==0);
	delete command;

	return success;
}

int	startConnections(sqlrconfigfile *cfgfile, int strace,
				const char *id, const char *config,
				const char *localstatedir,
				int connectiondebug) {

	// get the connection count and total metric
	connectstringnode	*connectionlist=cfgfile->getConnectStrings();

	// if the metrictotal was 0, start no connections
	if (!cfgfile->getMetricTotal()) {
		return 1;
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
	connectstringnode	*currentnode=connectionlist;
	int	metric=0;
	int	startup=0;
	int	totalstarted=0;
	int	done=0;
	while (!done) {
	
		// scale the number of each connection to start by 
		// each connection's metric vs the total number of 
		// connections to start up
		metric=currentnode->getMetric();
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
			done=1;
		}

		printf("\nStarting %d connections to ",startup);
		printf("%s :\n",currentnode->getConnectionId());

		// fire them up
		for (int i=0; i<startup; i++) {
	
			if (!startConnection(strace,cfgfile->getDbase(),
					id,currentnode->getConnectionId(),
					config,localstatedir,connectiondebug)) {
				return 0;
			}
			sleep(1);
		}

		// have we started enough connections?
		totalstarted=totalstarted+startup;
		if (totalstarted==connections) {
			done=1;
		}

		// next...
		if (currentnode) {
			currentnode=currentnode->getNext();
		}
	}
	return 1;
}

int	startScaler(sqlrconfigfile *cfgfile, char *id, char *config,
						char *localstatedir) {

	// don't start the scalar if unless dynamic scaling is enabled
	if (!cfgfile->getDynamicScaling()) {
		return 1;
	}

	printf("\nStarting scaler:\n");
	
	stringbuffer	*command=new stringbuffer();
	command->append("sqlr-scaler ")->append(" -id ")->append(id);
	command->append(" -config ")->append(config);
	if (localstatedir[0]) {
		command->append(" -localstatedir ")->append(localstatedir);
	}
	printf("  %s\n",command->getString());

	int	success=(system(command->getString())==0);
	delete command;

	return success;
}

int	startCacheManager(char *localstatedir) {

	// create a ps command that will detect if the cachemanager is running
	stringbuffer	*command=new stringbuffer();
	command->append(PS)->append(" | grep sqlr-cachemanager | grep -v grep");
	
	// run the command
	FILE	*cmd=popen(command->getString(),"r");
	delete command;
	
	// get the result
	stringbuffer	*contents=new stringbuffer();
	char		character;
	while (!feof(cmd) && (character=fgetc(cmd))>-1) {
		contents->append(character);
	}
	pclose(cmd);
	
	// if the cachemanger isn't running, start it
	if (!strlen(contents->getString())) {
	
		printf("\nStarting cache manager:\n");
	
		command=new stringbuffer();
		command->append("sqlr-cachemanager");
		if (localstatedir[0]) {
			command->append(" -cachedirs ")->append(localstatedir);
			command->append("/sqlrelay/cache");
		}
		printf("  %s\n",command->getString());

		int	success=(system(command->getString())==0);
		delete command;

		if (!success) {
			delete contents;
			return 0;
		}
	
	} else {
		printf("\ncache manager already running.\n");
	}
	
	delete contents;

	return 1;
}

int	main(int argc, const char **argv) {

	#include <version.h>

	// get the command line args
	cmdline	*cmdl=new cmdline(argc,argv);
	char	*localstatedir=cmdl->value("-localstatedir");
	int	strace=cmdl->found("-strace");
	char	*id=cmdl->getId();
	char	*config=cmdl->getConfig();

	// parse the config file(s)
	sqlrconfigfile	*cfgfile=new sqlrconfigfile();
	if (!cfgfile->parse(config,id)) {
		delete cfgfile;
		delete cmdl;
		exit(1);
	}

	// start listener, connections, scaler, cachemanager
	int	exitstatus=!(startListener(id,config,
				localstatedir,cfgfile->getDebugListener()) &&
			startConnections(cfgfile,strace,id,config,
				localstatedir,cfgfile->getDebugConnection()) &&
			startScaler(cfgfile,id,config,localstatedir) &&
			startCacheManager(localstatedir));

	// many thanks...
	printf("\n\nThanks to MP3.com for sponsoring: \n");
	printf("	Clustered/Replicated database support.\n");
	printf("	Perl API.\n");
	
	delete cfgfile;
	delete cmdl;

	// successful exit
	exit(exitstatus);
}
