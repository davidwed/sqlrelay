// Copyright (c) 2005  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <sqlrconfigfile.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

int main(int argc, const char **argv) {

	#include <version.h>

	sqlrconfigfile	cfgfile;
	usercontainer	*currentnode=NULL;

	commandline	cmdline(argc,argv);
	const char	*host;
	int16_t		port;
	const char	*socket;
	const char	*user;
	const char	*password;
	const char	*table="";
	bool		debug=false;
	int		exitval=0;

	const char	*config=cmdline.value("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	const char	*id=cmdline.value("-id");
	if (!(id && id[0])) {


		if (argc<7) {
			printf("usage: sqlr-import  host port socket "
				"user password table [debug] \n"
				"  or   sqlr-import  [-config configfile] "
				"-id id table [debug]\n");
			exit(1);
		}

		host=argv[1];
		port=charstring::toInteger(argv[2]);
		socket=argv[3];
		user=argv[4];
		password=argv[5];
		table=argv[6];
		if (argv[7] && !charstring::compare(argv[7],"debug")) {
			debug=true;
		}

	} else {

		if (cfgfile.parse(config,id)) {

			// get the host/port/socket/username/password
			host="localhost";
			port=cfgfile.getPort();
			socket=cfgfile.getUnixPort();
			// FIXME: this can return 0
			cfgfile.getUserList()->getDataByIndex(0,&currentnode);
			user=currentnode->getUser();
			password=currentnode->getPassword();

			// find the table and optional debug
			if (cmdline.found("debug")) {
				debug=true;
			}

			// find the table
			for (int i=1; i<argc; i++) {
				if (argv[i][0]=='-') {
					i++;
					continue;
				}
				if (!charstring::compare(argv[i],"debug")) {
					continue;
				}
				table=argv[i];
				break;
			}
		} else {
			exit(1);
		}
	}



	sqlrconnection	sqlrcon(host,port,socket,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	if (debug) {
		sqlrcon.debugOn();
	}

	exit(exitval);
}
