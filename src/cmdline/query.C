// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <sqlrconfigfile.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int	main(int argc, const char **argv) {

	#include <version.h>

	sqlrconfigfile	*cfgfile=NULL;
	usernode	*currentnode=NULL;

	commandline	*cmdline=new commandline(argc,argv);
	char		*host;
	int		port;
	char		*socket;
	char		*user;
	char		*password;
	char		*query="";
	int		debug=0;
	int		exitval=0;

	char	*config=cmdline->value("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	char	*id=cmdline->value("-id");
	if (!(id && id[0])) {


		if (argc<7) {
			printf("usage: query  host port socket ");
			printf("user password query [debug] \n");
			printf("  or   query  [-config configfile] ");
			printf("-id id query [debug]\n");
			exit(1);
		}

		host=(char *)argv[1];
		port=atoi(argv[2]);
		socket=(char *)argv[3];
		user=(char *)argv[4];
		password=(char *)argv[5];
		query=(char *)argv[6];
		if (argv[7] && !strcmp(argv[7],"debug")) {
			debug=1;
		}

	} else {

		cfgfile=new sqlrconfigfile();
		if (cfgfile->parse(config,id)) {

			// get the host/port/socket/username/password
			host="localhost";
			port=cfgfile->getPort();
			socket=cfgfile->getUnixPort();
			currentnode=cfgfile->getUsers();
			user=currentnode->getUser();
			password=currentnode->getPassword();

			// find the query and optional debug
			if (cmdline->found("debug")) {
				debug=1;
			}

			// find the query
			for (int i=1; i<argc; i++) {
				if (argv[i][0]=='-') {
					i++;
					continue;
				}
				if (!strcmp(argv[i],"debug")) {
					continue;
				}
				query=(char *)argv[i];
				break;
			}
		} else {
			delete cfgfile;
			delete cmdline;
			exit(1);
		}
	}



	sqlrconnection	*sqlrcon=new sqlrconnection(host,port,
						socket,user,password,0,1);
	if (debug) {
		sqlrcon->debugOn();
	}

	sqlrcursor	*sqlrcur=new sqlrcursor(sqlrcon);
	sqlrcur->dontGetColumnInfo();
	sqlrcur->setResultSetBufferSize(100);
	if (sqlrcur->sendQuery(query)) {
		int	i=0;
		int	cols=sqlrcur->colCount();
		char	*field="";
		while (cols && field) {
			for (int j=0; j<cols; j++) {
				if (field=sqlrcur->getField(i,j)) {
					printf("\"%s\"");
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
		printf("%s\n",sqlrcur->errorMessage());
		exitval=1;
	}
	sqlrcon->endSession();

	if (cfgfile) {
		delete cfgfile;
	}
	delete cmdline;
	delete sqlrcur;
	delete sqlrcon;

	exit(exitval);
}
