// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/stringbuffer.h>
#include <sqlrconfigfile.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

int main(int argc, const char **argv) {

	#include <version.h>

	commandline	cmdline(argc,argv);
	sqlrconfigfile	cfgfile;
	usercontainer	*currentnode=NULL;
	const char	*host;
	uint16_t	port;
	const char	*socket;
	const char	*user;
	const char	*password;
	const char	*table="";

	const char	*config=cmdline.getValue("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	const char	*id=cmdline.getValue("-id");

	if (!(id && id[0])) {

		if (argc<6) {
			printf("usage: fields  host port socket "
				"user password table\n"
				"  or   fields  [-config configfile] "
				"-id id table\n");
			exit(1);
		}

		host=argv[1];
		port=charstring::toInteger(argv[2]);
		socket=argv[3];
		user=argv[4];
		password=argv[5];
		table=argv[6];

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

			// find the query
			for (int i=1; i<argc; i++) {
				if (argv[i][0]=='-') {
					i++;
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

	stringbuffer	query;
	query.append("select * from ");
	query.append(table);
	query.append(" where rownum=1");

	sqlrcur.sendQuery(query.getString());
	sqlrcon.endSession();

	for (uint32_t j=0; j<sqlrcur.colCount(); j++) {
		printf("%s",sqlrcur.getColumnName(j));
		if (j<sqlrcur.colCount()-1) {
			printf(",");
		}
	}
	printf("\n");

	exit(0);
}
