// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <sqlrconfigfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	main(int argc, const char **argv) {

	#include <version.h>

	commandline	*cmdline=new commandline(argc,argv);
	sqlrconfigfile	*cfgfile=NULL;
	usernode	*currentnode=NULL;
	char		*host;
	int		port;
	char		*socket;
	char		*user;
	char		*password;
	char		*table="";

	char	*config=cmdline->value("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	char	*id=id=cmdline->value("-id");

	if (!(id && id[0])) {

		if (argc<6) {
			printf("usage: fields  host port socket ");
			printf("user password table\n");
			printf("  or   fields  [-config configfile] ");
			printf("-id id table\n");
			exit(1);
		}

		host=(char *)argv[1];
		port=atoi(argv[2]);
		socket=(char *)argv[3];
		user=(char *)argv[4];
		password=(char *)argv[5];
		table=(char *)argv[6];

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

			// find the query
			for (int i=1; i<argc; i++) {
				if (argv[i][0]=='-') {
					i++;
					continue;
				}
				table=(char *)argv[i];
				break;
			}
		} else {
			delete cfgfile;
			delete cmdline;
			exit(1);
		}
	}

	sqlrconnection	*sqlrcon=new sqlrconnection(host,port,socket,
						user,password,0,1);
	sqlrcursor	*sqlrcur=new sqlrcursor(sqlrcon);
	char	*query=new char[14+strlen(table)+15+1];
	sprintf(query,"select * from %s where rownum=1",table);
	sqlrcur->sendQuery(query);
	sqlrcon->endSession();
	delete[] query;

	for (int j=0; j<sqlrcur->colCount(); j++) {
		printf("%s",sqlrcur->getColumnName(j));
		if (j<sqlrcur->colCount()-1) {
			printf(",");
		}
	}

	if (cfgfile) {
		delete cfgfile;
	}
	if (cmdline) {
		delete cmdline;
	}
	delete sqlrcur;
	delete sqlrcon;

	exit(0);
}
