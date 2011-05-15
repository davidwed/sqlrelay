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
			printf("\"%s\"=\"%s\"\n",value,success);
			printf("failure ");
			delete cur;
			delete con;
			exit(0);
		}
	}

	if (!strcmp(value,success)) {
		printf("success ");
	} else {
		printf("\"%s\"=\"%s\"\n",value,success);
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
			printf("\"%s\"=\"%s\"\n",value,success);
			printf("failure ");
			delete cur;
			delete con;
			exit(0);
		}
	}

	if (!strncmp(value,success,length)) {
		printf("success ");
	} else {
		printf("\"%s\"=\"%s\"\n",value,success);
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
		printf("\"%d\"=\"%d\"\n",value,success);
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
		printf("\"%f\"=\"%f\"\n",value,success);
		printf("failure ");
		delete cur;
		delete con;
		exit(0);
	}
}

int	main(int argc, char **argv) {

	const char	*bindvars[6]={"1","2","3","4","5",NULL};
	const char	*bindvals[5]={"4","testchar4","testvarchar4","01-JAN-2004","testlong4"};
	const char	*subvars[4]={"var1","var2","var3",NULL};
	const char	*subvalstrings[3]={"hi","hello","bye"};
	int64_t		subvallongs[3]={1,2,3};
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
	con=new sqlrconnection("db.firstworks.com",9000,NULL,"test","test",0,1);
	cur=new sqlrcursor(con);

	printf("LONG CLOB: \n");
	cur->sendQuery("drop table testtable2");
	cur->sendQuery("create table testtable2 (testclob clob)");
	cur->prepareQuery("insert into testtable2 values (:clobval)");
	char	*clobval=new char[20*1024*1024+1];
	for (int i=0; i<20*1024*1024; i++) {
		clobval[i]='C';
	}
	clobval[20*1024*1024]=(char)NULL;
	cur->inputBindClob("clobval",clobval,20*1024*1024);
	checkSuccess(cur->executeQuery(),1);
	printf("\n");
	delete[] clobval;


	delete cur;
	delete con;

}
