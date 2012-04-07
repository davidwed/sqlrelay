// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include "../../config.h"

#include <sqlrelay/sqlrclient.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {

	if (argc<5) {
		printf("usage: inserttest host port socket user password\n");
		exit(0);
	}

	char	*host=argv[1];
	char	*port=argv[2];
	char	*socket=argv[3];
	char	*user=argv[4];
	char	*password=argv[5];

	time_t	starttime=time(NULL);
	printf("inserttest running, please wait...\n");
	clock();

	sqlrconnection	sqlrcon(host,atoi(port),socket,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);
	sqlrcur.sendQuery("drop table vis_acesso_usuario_programa");
	sqlrcur.sendQuery("create table vis_acesso_usuario_programa (nom_usuario char(8) not null , num_programa char(8) not null , opcao char(2) not null , ies_permite char(1), primary key (nom_usuario,num_programa,opcao))");
	stringbuffer	query;
	for (int qcount=0; qcount<1000; qcount++) {
		query.clear();
		query.append("insert into vis_acesso_usuario_programa values ('")->append(qcount)->append("','hello','he','h')");
		sqlrcur.sendQuery(query.getString());
	}

	printf("total system time used : %ld\n",clock());
	printf("total real time : %ld\n",time(NULL)-starttime);

	sqlrcur.sendQuery("drop table vis_acesso_usuario_programa");
}
