// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#include <stdlib.h>
#include <sqlrelay/sqlrclient.h>
#include <stdio.h>
#include <pthread.h>
#include <config.h>

const char	*host;
int		port;
const char	*sock;
const char	*login;
const char	*password;
const char	*query;
int		threadcount;

void	runQuery(void *id) {

	sqlrconnection	*con=new sqlrconnection(host,port,sock,
						login,password,0,1);
	sqlrcursor	*cur=new sqlrcursor(con);

	con->debugOn();
	cur->sendQuery(query);
	con->endSession();
	
	for (int i=0; i<cur->rowCount(); i++) {
		printf("%d  ",(uint64_t)id);
		for (int j=0; j<cur->colCount(); j++) {
			printf("\"%s\",",cur->getField(i,j));
		}
		printf("\n");
	}

	delete cur;
	delete con;
}

main(int argc, char **argv) {

	host="localhost";
	port=9000;
	sock="/tmp/test.socket";
	login="test";
	password="test";
	query=argv[1];
	threadcount=atoi(argv[2]);

	pthread_t	th[threadcount];

	for (int i=0; i<threadcount; i++) {
		pthread_create(&th[i],NULL,
			(void *(*)(void *))runQuery,
			(void *)i);
	}

	for (int i=0; i<threadcount; i++) {
		pthread_join(th[i],NULL);
	}
}
