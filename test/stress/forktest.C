// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#include <stdlib.h>
#include <sqlrelay/sqlrclient.h>
#include <stdio.h>
#include <unistd.h>
#include <config.h>

char	*host;
int	port;
char	*socket;
char	*login;
char	*password;
char	*query;
int	forkcount;

void	runQuery(int id) {

	sqlrconnection	*con=new sqlrconnection(host,port,socket,
						login,password,0,1);
	sqlrcursor	*cur=new sqlrcursor(con);

	con->debugOn();
	cur->sendQuery(query);
	con->endSession();
	
	for (int i=0; i<cur->rowCount(); i++) {
		printf("%d  ",(int)id);
		for (int j=0; j<cur->colCount(); j++) {
			printf("\"%s\",",cur->getField(i,j));
		}
		printf("\n");
	}

	delete cur;
	delete con;
}

main(int argc, char **argv) {

	host=argv[1];
	port=atoi(argv[2]);
	socket=argv[3];
	login=argv[4];
	password=argv[5];
	query=argv[6];
	forkcount=atoi(argv[7]);

	for (int i=0; i<forkcount; i++) {
		if (fork()) {
			runQuery(i);
			_exit(0);
		}
	}
}
