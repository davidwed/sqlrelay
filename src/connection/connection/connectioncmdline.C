#include <defaults.h>
#include <connection/connectioncmdline.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <config.h>

connectioncmdline::connectioncmdline(int argc, const char **argv) :
						cmdline(argc,argv) {
	setConnectionId();
	setTtl();
}

void	connectioncmdline::setConnectionId() {

	// get the connection id from the command line
	connectionid=value("-connectionid");
	if (!connectionid[0]) {
		connectionid=DEFAULT_CONNECTIONID;
		fprintf(stderr,"Warning: using default connectionid.\n");
	}
}

void	connectioncmdline::setTtl() {

	// get the time to live from the command line
	ttl=atoi(value("-ttl"));
}
