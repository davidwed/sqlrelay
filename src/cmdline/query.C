// Copyright (c) 1999-2001  David Muse
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
	const char	*query="";
	bool		debug=false;
	int		exitval=0;

	const char	*config=cmdline.value("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	const char	*id=cmdline.value("-id");
	if (!(id && id[0])) {


		if (argc<7) {
			printf("usage: query  host port socket ");
			printf("user password query [debug] \n");
			printf("  or   query  [-config configfile] ");
			printf("-id id query [debug]\n");
			exit(1);
		}

		host=argv[1];
		port=charstring::toLong(argv[2]);
		socket=argv[3];
		user=argv[4];
		password=argv[5];
		query=argv[6];
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

			// find the query and optional debug
			if (cmdline.found("debug")) {
				debug=true;
			}

			// find the query
			for (int i=1; i<argc; i++) {
				if (argv[i][0]=='-') {
					i++;
					continue;
				}
				if (!charstring::compare(argv[i],"debug")) {
					continue;
				}
				query=argv[i];
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

	sqlrcur.dontGetColumnInfo();
	sqlrcur.setResultSetBufferSize(100);
	if (sqlrcur.sendQuery(query)) {
		int		i=0;
		int		cols=sqlrcur.colCount();
		const char	*field="";
		while (cols && field) {
			for (int j=0; j<cols; j++) {
				if ((field=sqlrcur.getField(i,j))) {
					printf("\"%s\"",field);
					if (j<cols-1) {
						printf(",");
					} else {
						printf("\n");
					}
				}
			}
			i++;
		}
	} else {
		printf("%s\n",sqlrcur.errorMessage());
		exitval=1;
	}
	sqlrcon.endSession();

	exit(exitval);
}
