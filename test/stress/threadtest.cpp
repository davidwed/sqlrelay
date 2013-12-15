// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/thread.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>

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
	
	for (uint64_t i=0; i<cur->rowCount(); i++) {
		stdoutput.printf("%d  ",(int)id);
		for (uint32_t j=0; j<cur->colCount(); j++) {
			stdoutput.printf("\"%s\",",cur->getField(i,j));
		}
		stdoutput.printf("\n");
	}

	delete cur;
	delete con;
}

int main(int argc, char **argv) {

	if (argc<3) {
		stdoutput.printf("usage: threadtest \"query\" threadcount\n");
		process::exit(1);
	}

	host="localhost";
	port=9000;
	sock="/tmp/test.socket";
	login="test";
	password="test";
	query=argv[1];
	threadcount=charstring::toInteger(argv[2]);

	thread	th[threadcount];

	for (int i=0; i<threadcount; i++) {
		th[i].setFunction((void *(*)(void *))runQuery,(void *)i);
		th[i].create();
	}

	for (int i=0; i<threadcount; i++) {
		th[i].join(NULL);
	}
	process::exit(1);
}
