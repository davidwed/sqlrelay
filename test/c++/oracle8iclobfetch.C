// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/datetime.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("failure ");
			delete cur;
			delete con;
			exit(0);
		}
	}

	if (!strcmp(value,success)) {
		printf("success ");
	} else {
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

void checkSuccess(const char *value, const char *success, size_t length) {

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("failure ");
			delete cur;
			delete con;
			exit(0);
		}
	}

	if (!strncmp(value,success,length)) {
		printf("success ");
	} else {
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

void checkSuccess(double value, double success) {

	if (value==success) {
		printf("success ");
	} else {
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

int	main(int argc, char **argv) {

	char		*dbtype;
	const char	*bindvars[6]={"1","2","3","4","5",NULL};
	const char	*bindvals[5]={"4","testchar4","testvarchar4","01-JAN-2004","testlong4"};
	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	int32_t		subvallongs[3]={1,2,3};
	double		subvaldoubles[3]={10.55,10.556,10.5556};
	uint32_t	precs[3]={4,5,6};
	uint32_t	scales[3]={2,3,4};
	const char	*numvar;
	const char	*clobvar;
	uint32_t	clobvarlength;
	const char	*blobvar;
	uint32_t	blobvarlength;
	const char	*stringvar;
	const char	*floatvar;
	const char * const *cols;
	const char * const *fields;
	uint16_t	port;
	const char	*socket;
	uint16_t	id;
	const char	*filename;
	const char	*arraybindvars[6]={"var1","var2","var3","var4","var5",NULL};
	const char	*arraybindvals[5]={"7","testchar7","testvarchar7","01-JAN-2007","testlong7"};
	uint32_t	*fieldlens;


	// usage...
	if (argc<5) {
		printf("usage: oracle8i host port socket user password\n");
		exit(0);
	}


	// instantiation
	con=new sqlrconnection(argv[1],atoi(argv[2]), 
					argv[3],argv[4],argv[5],0,1);
	cur=new sqlrcursor(con);

	printf("LONG CLOB: \n");
	cur->prepareQuery("begin select testclob into :clobbindval from testtable2; end;");
	cur->defineOutputBindClob("clobbindval");
	checkSuccess(cur->executeQuery(),1);
	const char	*clobbindvar=cur->getOutputBind("clobbindval");
	checkSuccess(cur->getOutputBindLength("clobbindval"),20*1024*1024);
	printf("\n");


	delete cur;
	delete con;

}
