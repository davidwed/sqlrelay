// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

sqlrconnection	*con;
sqlrcursor	*cur;
sqlrconnection	*secondcon;
sqlrcursor	*secondcur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("%s!=%s\n",value,success);
			stdoutput.printf("failure ");
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%s!=%s\n",value,success);
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(const char *value, const char *success, size_t length) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("%s!=%s\n",value,success);
			stdoutput.printf("failure ");
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success,length)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%s!=%s\n",value,success);
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%d!=%d\n",value,success);
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(double value, double success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("%f!=%f\n",value,success);
		stdoutput.printf("failure ");
		delete cur;
		delete con;
		process::exit(1);
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
	int64_t		numvar;
	const char	*clobvar;
	uint32_t	clobvarlength;
	const char	*blobvar;
	uint32_t	blobvarlength;
	const char	*stringvar;
	double		floatvar;
	const char * const *cols;
	const char * const *fields;
	uint16_t	port;
	const char	*socket;
	uint16_t	id;
	const char	*filename;
	const char	*arraybindvars[6]={"var1","var2","var3","var4","var5",NULL};
	const char	*arraybindvals[5]={"7","testchar7","testvarchar7","01-JAN-2007","testlong7"};
	uint32_t	*fieldlens;

	const char	*cert="/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem";
	const char	*ca="/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem";
	#ifdef _WIN32
		cert="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\client.pfx";
		ca="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\ca.pfx";
	#endif

	// instantiation
	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							NULL,NULL,0,1);
	cur=new sqlrcursor(con);
	con->enableTls(NULL,cert,NULL,NULL,"ca",ca,0);

	// get database type
	stdoutput.printf("IDENTIFY: \n");
	checkSuccess(con->identify(),"oracle");
	stdoutput.printf("\n");

	// ping
	stdoutput.printf("PING: \n");
	checkSuccess(con->ping(),1);
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");

	stdoutput.printf("CREATE TEMPTABLE: \n");
	checkSuccess(cur->sendQuery("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date, testlong long, testclob clob, testblob blob)"),1);
	stdoutput.printf("\n");

	stdoutput.printf("INSERT: \n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001','testlong1','testclob1',empty_blob())"),1);
	checkSuccess(cur->countBindVariables(),0);
	stdoutput.printf("\n");

	stdoutput.printf("AFFECTED ROWS: \n");
	checkSuccess(cur->affectedRows(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY POSITION: \n");
	cur->prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	checkSuccess(cur->countBindVariables(),7);
	cur->inputBind("1",2);
	cur->inputBind("2","testchar2");
	cur->inputBind("3","testvarchar2");
	cur->inputBind("4",2002,1,1,0,0,0,0,NULL,false);
	cur->inputBind("5","testlong2");
	cur->inputBindClob("6","testclob2",9);
	cur->inputBindBlob("7","testblob2",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("1",3);
	cur->inputBind("2","testchar3");
	cur->inputBind("3","testvarchar3");
	cur->inputBind("4",2003,1,1,0,0,0,0,NULL,false);
	cur->inputBind("5","testlong3");
	cur->inputBindClob("6","testclob3",9);
	cur->inputBindBlob("7","testblob3",9);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY OF BINDS BY POSITION: \n");
	cur->clearBinds();
	cur->inputBinds(bindvars,bindvals);
	cur->inputBindClob("6","testclob4",9);
	cur->inputBindBlob("7","testblob4",9);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY NAME: \n");
	cur->prepareQuery("insert into testtable values (:var1,:var2,:var3,:var4,:var5,:var6,:var7)");
	cur->inputBind("var1",5);
	cur->inputBind("var2","testchar5");
	cur->inputBind("var3","testvarchar5");
	cur->inputBind("var4",2005,1,1,0,0,0,0,NULL,false);
	cur->inputBind("var5","testlong5");
	cur->inputBindClob("var6","testclob5",9);
	cur->inputBindBlob("var7","testblob5",9);
	checkSuccess(cur->executeQuery(),1);
	cur->clearBinds();
	cur->inputBind("var1",6);
	cur->inputBind("var2","testchar6");
	cur->inputBind("var3","testvarchar6");
	cur->inputBind("var4",2006,1,1,0,0,0,0,NULL,false);
	cur->inputBind("var5","testlong6");
	cur->inputBindClob("var6","testclob6",9);
	cur->inputBindBlob("var7","testblob6",9);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY OF BINDS BY NAME: \n");
	cur->clearBinds();
	cur->inputBinds(arraybindvars,arraybindvals);
	cur->inputBindClob("var6","testclob7",9);
	cur->inputBindBlob("var7","testblob7",9);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->inputBind("var1",8);
	cur->inputBind("var2","testchar8");
	cur->inputBind("var3","testvarchar8");
	cur->inputBind("var4",2008,1,1,0,0,0,0,NULL,false);
	cur->inputBind("var5","testlong8");
	cur->inputBindClob("var6","testclob8",9);
	cur->inputBindBlob("var7","testblob8",9);
	cur->inputBind("var9","junkvalue");
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("OUTPUT BIND BY NAME: \n");
	cur->prepareQuery("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; :datevar:='03-FEB-2001'; end;");
	cur->defineOutputBindInteger("numvar");
	cur->defineOutputBindString("stringvar",10);
	cur->defineOutputBindDouble("floatvar");
	cur->defineOutputBindDate("datevar");
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBindInteger("numvar");
	stringvar=cur->getOutputBindString("stringvar");
	floatvar=cur->getOutputBindDouble("floatvar");
	int16_t	year=0;
	int16_t	month=0;
	int16_t	day=0;
	int16_t	hour=0;
	int16_t	minute=0;
	int16_t	second=0;
	int32_t	microsecond=0;
	bool	isnegative=false;
	const char	*tz=NULL;
	cur->getOutputBindDate("datevar",&year,&month,&day,
					&hour,&minute,&second,&microsecond,&tz,
					&isnegative);
	checkSuccess(numvar,1);
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,2.5);
	checkSuccess(year,2001);
	checkSuccess(month,2);
	checkSuccess(day,3);
	checkSuccess(hour,0);
	checkSuccess(minute,0);
	checkSuccess(second,0);
	checkSuccess(microsecond,0);
	checkSuccess(tz,"");
	stdoutput.printf("\n");

	stdoutput.printf("OUTPUT BIND BY POSITION: \n");
	cur->clearBinds();
	cur->defineOutputBindInteger("1");
	cur->defineOutputBindString("2",10);
	cur->defineOutputBindDouble("3");
	cur->defineOutputBindDate("4");
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBindInteger("1");
	stringvar=cur->getOutputBindString("2");
	floatvar=cur->getOutputBindDouble("3");
	cur->getOutputBindDate("4",&year,&month,&day,
					&hour,&minute,&second,&microsecond,&tz,
					&isnegative);
	checkSuccess(numvar,1);
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,2.5);
	checkSuccess(year,2001);
	checkSuccess(month,2);
	checkSuccess(day,3);
	checkSuccess(hour,0);
	checkSuccess(minute,0);
	checkSuccess(second,0);
	checkSuccess(microsecond,0);
	checkSuccess(tz,"");
	stdoutput.printf("\n");

	stdoutput.printf("OUTPUT BIND BY NAME WITH VALIDATION: \n");
	cur->clearBinds();
	cur->defineOutputBindInteger("numvar");
	cur->defineOutputBindString("stringvar",10);
	cur->defineOutputBindDouble("floatvar");
	cur->defineOutputBindDate("datevar");
	cur->defineOutputBindString("dummyvar",10);
	cur->validateBinds();
	checkSuccess(cur->executeQuery(),1);
	numvar=cur->getOutputBindInteger("numvar");
	stringvar=cur->getOutputBindString("stringvar");
	floatvar=cur->getOutputBindDouble("floatvar");
	cur->getOutputBindDate("datevar",&year,&month,&day,
				&hour,&minute,&second,&microsecond,&tz,
				&isnegative);
	checkSuccess(numvar,1);
	checkSuccess(stringvar,"hello");
	checkSuccess(floatvar,2.5);
	checkSuccess(year,2001);
	checkSuccess(month,2);
	checkSuccess(day,3);
	checkSuccess(hour,0);
	checkSuccess(minute,0);
	checkSuccess(second,0);
	checkSuccess(microsecond,0);
	checkSuccess(tz,"");
	stdoutput.printf("\n");

	stdoutput.printf("SELECT: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT: \n");
	checkSuccess(cur->colCount(),7);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES: \n");
	checkSuccess(cur->getColumnName(0),"TESTNUMBER");
	checkSuccess(cur->getColumnName(1),"TESTCHAR");
	checkSuccess(cur->getColumnName(2),"TESTVARCHAR");
	checkSuccess(cur->getColumnName(3),"TESTDATE");
	checkSuccess(cur->getColumnName(4),"TESTLONG");
	checkSuccess(cur->getColumnName(5),"TESTCLOB");
	checkSuccess(cur->getColumnName(6),"TESTBLOB");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"TESTNUMBER");
	checkSuccess(cols[1],"TESTCHAR");
	checkSuccess(cols[2],"TESTVARCHAR");
	checkSuccess(cols[3],"TESTDATE");
	checkSuccess(cols[4],"TESTLONG");
	checkSuccess(cols[5],"TESTCLOB");
	checkSuccess(cols[6],"TESTBLOB");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN TYPES: \n");
	checkSuccess(cur->getColumnType((uint32_t)0),"NUMBER");
	checkSuccess(cur->getColumnType("TESTNUMBER"),"NUMBER");
	checkSuccess(cur->getColumnType(1),"CHAR");
	checkSuccess(cur->getColumnType("TESTCHAR"),"CHAR");
	checkSuccess(cur->getColumnType(2),"VARCHAR2");
	checkSuccess(cur->getColumnType("TESTVARCHAR"),"VARCHAR2");
	checkSuccess(cur->getColumnType(3),"DATE");
	checkSuccess(cur->getColumnType("TESTDATE"),"DATE");
	checkSuccess(cur->getColumnType(4),"LONG");
	checkSuccess(cur->getColumnType("TESTLONG"),"LONG");
	checkSuccess(cur->getColumnType(5),"CLOB");
	checkSuccess(cur->getColumnType("TESTCLOB"),"CLOB");
	checkSuccess(cur->getColumnType(6),"BLOB");
	checkSuccess(cur->getColumnType("TESTBLOB"),"BLOB");
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN LENGTH: \n");
	checkSuccess(cur->getColumnLength((uint32_t)0),22);
	checkSuccess(cur->getColumnLength("TESTNUMBER"),22);
	checkSuccess(cur->getColumnLength(1),40);
	checkSuccess(cur->getColumnLength("TESTCHAR"),40);
	checkSuccess(cur->getColumnLength(2),40);
	checkSuccess(cur->getColumnLength("TESTVARCHAR"),40);
	checkSuccess(cur->getColumnLength(3),7);
	checkSuccess(cur->getColumnLength("TESTDATE"),7);
	checkSuccess(cur->getColumnLength(4),0);
	checkSuccess(cur->getColumnLength("TESTLONG"),0);
	checkSuccess(cur->getColumnLength(5),0);
	checkSuccess(cur->getColumnLength("TESTCLOB"),0);
	checkSuccess(cur->getColumnLength(6),0);
	checkSuccess(cur->getColumnLength("TESTBLOB"),0);
	stdoutput.printf("\n");

	stdoutput.printf("LONGEST COLUMN: \n");
	checkSuccess(cur->getLongest((uint32_t)0),1);
	checkSuccess(cur->getLongest("TESTNUMBER"),1);
	checkSuccess(cur->getLongest(1),40);
	checkSuccess(cur->getLongest("TESTCHAR"),40);
	checkSuccess(cur->getLongest(2),12);
	checkSuccess(cur->getLongest("TESTVARCHAR"),12);
	checkSuccess(cur->getLongest(3),9);
	checkSuccess(cur->getLongest("TESTDATE"),9);
	checkSuccess(cur->getLongest(4),9);
	checkSuccess(cur->getLongest("TESTLONG"),9);
	checkSuccess(cur->getLongest(5),9);
	checkSuccess(cur->getLongest("TESTCLOB"),9);
	checkSuccess(cur->getLongest(6),9);
	checkSuccess(cur->getLongest("TESTBLOB"),9);
	stdoutput.printf("\n");

	stdoutput.printf("ROW COUNT: \n");
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("TOTAL ROWS: \n");
	checkSuccess(cur->totalRows(),0);
	stdoutput.printf("\n");

	stdoutput.printf("FIRST ROW INDEX: \n");
	checkSuccess(cur->firstRowIndex(),0);
	stdoutput.printf("\n");

	stdoutput.printf("END OF RESULT SET: \n");
	checkSuccess(cur->endOfResultSet(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY INDEX: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"testchar1                               ");
	checkSuccess(cur->getField(0,2),"testvarchar1");
	checkSuccess(cur->getField(0,3),"01-JAN-01");
	checkSuccess(cur->getField(0,4),"testlong1");
	checkSuccess(cur->getField(0,5),"testclob1");
	checkSuccess(cur->getField(0,6),"");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(7,1),"testchar8                               ");
	checkSuccess(cur->getField(7,2),"testvarchar8");
	checkSuccess(cur->getField(7,3),"01-JAN-08");
	checkSuccess(cur->getField(7,4),"testlong8");
	checkSuccess(cur->getField(7,5),"testclob8");
	checkSuccess(cur->getField(7,6),"testblob8");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY INDEX: \n");
	checkSuccess(cur->getFieldLength(0,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(0,1),40);
	checkSuccess(cur->getFieldLength(0,2),12);
	checkSuccess(cur->getFieldLength(0,3),9);
	checkSuccess(cur->getFieldLength(0,4),9);
	checkSuccess(cur->getFieldLength(0,5),9);
	checkSuccess(cur->getFieldLength(0,6),0);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,(uint32_t)0),1);
	checkSuccess(cur->getFieldLength(7,1),40);
	checkSuccess(cur->getFieldLength(7,2),12);
	checkSuccess(cur->getFieldLength(7,3),9);
	checkSuccess(cur->getFieldLength(7,4),9);
	checkSuccess(cur->getFieldLength(7,5),9);
	checkSuccess(cur->getFieldLength(7,6),9);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY NAME: \n");
	checkSuccess(cur->getField(0,"TESTNUMBER"),"1");
	checkSuccess(cur->getField(0,"TESTCHAR"),"testchar1                               ");
	checkSuccess(cur->getField(0,"TESTVARCHAR"),"testvarchar1");
	checkSuccess(cur->getField(0,"TESTDATE"),"01-JAN-01");
	checkSuccess(cur->getField(0,"TESTLONG"),"testlong1");
	checkSuccess(cur->getField(0,"TESTCLOB"),"testclob1");
	checkSuccess(cur->getField(0,"TESTBLOB"),"");
	stdoutput.printf("\n");
	checkSuccess(cur->getField(7,"TESTNUMBER"),"8");
	checkSuccess(cur->getField(7,"TESTCHAR"),"testchar8                               ");
	checkSuccess(cur->getField(7,"TESTVARCHAR"),"testvarchar8");
	checkSuccess(cur->getField(7,"TESTDATE"),"01-JAN-08");
	checkSuccess(cur->getField(7,"TESTLONG"),"testlong8");
	checkSuccess(cur->getField(7,"TESTCLOB"),"testclob8");
	checkSuccess(cur->getField(7,"TESTBLOB"),"testblob8");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY NAME: \n");
	checkSuccess(cur->getFieldLength(0,"TESTNUMBER"),1);
	checkSuccess(cur->getFieldLength(0,"TESTCHAR"),40);
	checkSuccess(cur->getFieldLength(0,"TESTVARCHAR"),12);
	checkSuccess(cur->getFieldLength(0,"TESTDATE"),9);
	checkSuccess(cur->getFieldLength(0,"TESTLONG"),9);
	checkSuccess(cur->getFieldLength(0,"TESTCLOB"),9);
	checkSuccess(cur->getFieldLength(0,"TESTBLOB"),0);
	stdoutput.printf("\n");
	checkSuccess(cur->getFieldLength(7,"TESTNUMBER"),1);
	checkSuccess(cur->getFieldLength(7,"TESTCHAR"),40);
	checkSuccess(cur->getFieldLength(7,"TESTVARCHAR"),12);
	checkSuccess(cur->getFieldLength(7,"TESTDATE"),9);
	checkSuccess(cur->getFieldLength(7,"TESTLONG"),9);
	checkSuccess(cur->getFieldLength(7,"TESTCLOB"),9);
	checkSuccess(cur->getFieldLength(7,"TESTBLOB"),9);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS BY ARRAY: \n");
	fields=cur->getRow(0);
	checkSuccess(fields[0],"1");
	checkSuccess(fields[1],"testchar1                               ");
	checkSuccess(fields[2],"testvarchar1");
	checkSuccess(fields[3],"01-JAN-01");
	checkSuccess(fields[4],"testlong1");
	checkSuccess(fields[5],"testclob1");
	checkSuccess(fields[6],"");
	stdoutput.printf("\n");

	stdoutput.printf("FIELD LENGTHS BY ARRAY: \n");
	fieldlens=cur->getRowLengths(0);
	checkSuccess(fieldlens[0],1);
	checkSuccess(fieldlens[1],40);
	checkSuccess(fieldlens[2],12);
	checkSuccess(fieldlens[3],9);
	checkSuccess(fieldlens[4],9);
	checkSuccess(fieldlens[5],9);
	checkSuccess(fieldlens[6],0);
	stdoutput.printf("\n");

	stdoutput.printf("INDIVIDUAL SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),'$(var2)',$(var3) from dual");
	cur->substitution("var1",1);
	cur->substitution("var2","hello");
	cur->substitution("var3",10.5556,6,4);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"10.5556");
	stdoutput.printf("\n");

	stdoutput.printf("OUTPUT BIND: \n");
	cur->prepareQuery("begin :var1:='hello'; end;");
	cur->defineOutputBindString("var1",10);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindString("var1"),"hello");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3) from dual");
	cur->substitutions(subvars,subvallongs);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");
	
	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(0,1),"2");
	checkSuccess(cur->getField(0,2),"3");
	stdoutput.printf("\n");
	
	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select '$(var1)','$(var2)','$(var3)' from dual");
	cur->substitutions(subvars,subvalstrings);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"hi");
	checkSuccess(cur->getField(0,1),"hello");
	checkSuccess(cur->getField(0,2),"bye");
	stdoutput.printf("\n");

	stdoutput.printf("ARRAY SUBSTITUTIONS: \n");
	cur->prepareQuery("select $(var1),$(var2),$(var3) from dual");
	cur->substitutions(subvars,subvaldoubles,precs,scales);
	checkSuccess(cur->executeQuery(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FIELDS: \n");
	checkSuccess(cur->getField(0,(uint32_t)0),"10.55");
	checkSuccess(cur->getField(0,1),"10.556");
	checkSuccess(cur->getField(0,2),"10.5556");
	stdoutput.printf("\n");

	stdoutput.printf("NULLS as Nulls: \n");
	cur->getNullsAsNulls();
	checkSuccess(cur->sendQuery("select NULL,1,NULL from dual"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),NULL);
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),NULL);
	cur->getNullsAsEmptyStrings();
	checkSuccess(cur->sendQuery("select NULL,1,NULL from dual"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"");
	checkSuccess(cur->getField(0,1),"1");
	checkSuccess(cur->getField(0,2),"");
	cur->getNullsAsNulls();
	stdoutput.printf("\n");

	stdoutput.printf("RESULT SET BUFFER SIZE: \n");
	checkSuccess(cur->getResultSetBufferSize(),0);
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getResultSetBufferSize(),2);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),0);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),2);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->firstRowIndex(),2);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),4);
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	stdoutput.printf("\n");

	stdoutput.printf("DONT GET COLUMN INFO: \n");
	cur->dontGetColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getColumnName(0),NULL);
	checkSuccess(cur->getColumnLength((uint32_t)0),0);
	checkSuccess(cur->getColumnType((uint32_t)0),NULL);
	cur->getColumnInfo();
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getColumnName(0),"TESTNUMBER");
	checkSuccess(cur->getColumnLength((uint32_t)0),22);
	checkSuccess(cur->getColumnType((uint32_t)0),"NUMBER");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	delete[] socket;
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	delete[] socket;
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	delete[] socket;
	stdoutput.printf("\n");
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(cur->getField(1,(uint32_t)0),"2");
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	checkSuccess(cur->getField(3,(uint32_t)0),"4");
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");

	stdoutput.printf("SUSPENDED RESULT SET: \n");
	cur->setResultSetBufferSize(2);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	delete[] socket;
	checkSuccess(cur->resumeResultSet(id),1);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),4);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),6);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->setResultSetBufferSize(0);
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET: \n");
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN COUNT FOR CACHED RESULT SET: \n");
	checkSuccess(cur->colCount(),7);
	stdoutput.printf("\n");

	stdoutput.printf("COLUMN NAMES FOR CACHED RESULT SET: \n");
	checkSuccess(cur->getColumnName(0),"TESTNUMBER");
	checkSuccess(cur->getColumnName(1),"TESTCHAR");
	checkSuccess(cur->getColumnName(2),"TESTVARCHAR");
	checkSuccess(cur->getColumnName(3),"TESTDATE");
	checkSuccess(cur->getColumnName(4),"TESTLONG");
	checkSuccess(cur->getColumnName(5),"TESTCLOB");
	checkSuccess(cur->getColumnName(6),"TESTBLOB");
	cols=cur->getColumnNames();
	checkSuccess(cols[0],"TESTNUMBER");
	checkSuccess(cols[1],"TESTCHAR");
	checkSuccess(cols[2],"TESTVARCHAR");
	checkSuccess(cols[3],"TESTDATE");
	checkSuccess(cols[4],"TESTLONG");
	checkSuccess(cols[5],"TESTCLOB");
	checkSuccess(cols[6],"TESTBLOB");
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("FROM ONE CACHE FILE TO ANOTHER: \n");
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");

	stdoutput.printf("FROM ONE CACHE FILE TO ANOTHER WITH RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile2");
	checkSuccess(cur->openCachedResultSet("cachefile1"),1);
	cur->cacheOff();
	checkSuccess(cur->openCachedResultSet("cachefile2"),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	stdoutput.printf("\n");

	stdoutput.printf("CACHED RESULT SET WITH SUSPEND AND RESULT SET BUFFER SIZE: \n");
	cur->setResultSetBufferSize(2);
	cur->cacheToFile("cachefile1");
	cur->setCacheTtl(200);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getField(2,(uint32_t)0),"3");
	filename=charstring::duplicate(cur->getCacheFileName());
	checkSuccess(filename,"cachefile1");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	stdoutput.printf("\n");
	checkSuccess(con->resumeSession(port,socket),1);
	delete[] socket;
	checkSuccess(cur->resumeCachedResultSet(id,filename),1);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),4);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),6);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),6);
	checkSuccess(cur->endOfResultSet(),0);
	checkSuccess(cur->rowCount(),8);
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	stdoutput.printf("\n");
	checkSuccess(cur->firstRowIndex(),8);
	checkSuccess(cur->endOfResultSet(),1);
	checkSuccess(cur->rowCount(),8);
	cur->cacheOff();
	stdoutput.printf("\n");
	checkSuccess(cur->openCachedResultSet(filename),1);
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	checkSuccess(cur->getField(8,(uint32_t)0),NULL);
	cur->setResultSetBufferSize(0);
	delete[] filename;
	stdoutput.printf("\n");

	stdoutput.printf("COMMIT AND ROLLBACK: \n");
	secondcon=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
							NULL,NULL,0,1);
	secondcur=new sqlrcursor(secondcon);
	secondcon->enableTls(NULL,cert,NULL,NULL,"ca",ca,0);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"0");
	checkSuccess(con->commit(),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"8");
	checkSuccess(con->autoCommitOn(),1);
	checkSuccess(cur->sendQuery("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010','testlong10','testclob10',NULL)"),1);
	checkSuccess(secondcur->sendQuery("select count(*) from testtable"),1);
	checkSuccess(secondcur->getField(0,(uint32_t)0),"9");
	checkSuccess(con->autoCommitOff(),1);
	stdoutput.printf("\n");

	stdoutput.printf("FINISHED SUSPENDED SESSION: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),1);
	checkSuccess(cur->getField(4,(uint32_t)0),"5");
	checkSuccess(cur->getField(5,(uint32_t)0),"6");
	checkSuccess(cur->getField(6,(uint32_t)0),"7");
	checkSuccess(cur->getField(7,(uint32_t)0),"8");
	id=cur->getResultSetId();
	cur->suspendResultSet();
	checkSuccess(con->suspendSession(),1);
	port=con->getConnectionPort();
	socket=charstring::duplicate(con->getConnectionSocket());
	checkSuccess(con->resumeSession(port,socket),1);
	delete[] socket;
	checkSuccess(cur->resumeResultSet(id),1);
	checkSuccess(cur->getField(4,(uint32_t)0),NULL);
	checkSuccess(cur->getField(5,(uint32_t)0),NULL);
	checkSuccess(cur->getField(6,(uint32_t)0),NULL);
	checkSuccess(cur->getField(7,(uint32_t)0),NULL);
	stdoutput.printf("\n");

	stdoutput.printf("CLOB AND BLOB OUTPUT BIND: \n");
	cur->sendQuery("drop table testtable1");
	checkSuccess(cur->sendQuery("create table testtable1 (testclob clob, testblob blob)"),1);
	cur->prepareQuery("insert into testtable1 values ('hello',:var1)");
	cur->inputBindBlob("var1","hello",5);
	checkSuccess(cur->executeQuery(),1);
	cur->prepareQuery("begin select testclob into :clobvar from testtable1;  select testblob into :blobvar from testtable1; end;");
	cur->defineOutputBindClob("clobvar");
	cur->defineOutputBindBlob("blobvar");
	checkSuccess(cur->executeQuery(),1);
	clobvar=cur->getOutputBindClob("clobvar");
	clobvarlength=cur->getOutputBindLength("clobvar");
	blobvar=cur->getOutputBindBlob("blobvar");
	blobvarlength=cur->getOutputBindLength("blobvar");
	checkSuccess(clobvar,"hello",5);
	checkSuccess(clobvarlength,5);
	checkSuccess(blobvar,"hello",5);
	checkSuccess(blobvarlength,5);
	cur->sendQuery("drop table testtable1");
	stdoutput.printf("\n");

	stdoutput.printf("NULL AND EMPTY CLOBS AND BLOBS: \n");
	cur->getNullsAsNulls();
	cur->sendQuery("create table testtable1 (testclob1 clob, testclob2 clob, testblob1 blob, testblob2 blob)");
	cur->prepareQuery("insert into testtable1 values (:var1,:var2,:var3,:var4)");
	cur->inputBindClob("var1","",0);
	cur->inputBindClob("var2",NULL,0);
	cur->inputBindBlob("var3","",0);
	cur->inputBindBlob("var4",NULL,0);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select * from testtable1");
	checkSuccess(cur->getField(0,(uint32_t)0),NULL);
	checkSuccess(cur->getField(0,1),NULL);
	checkSuccess(cur->getField(0,2),NULL);
	checkSuccess(cur->getField(0,3),NULL);
	cur->sendQuery("drop table testtable1");
	stdoutput.printf("\n");

	stdoutput.printf("CURSOR BINDS: \n");
	cur->clearBinds();
	checkSuccess(cur->sendQuery("create or replace package types is type cursorType is ref cursor; end;"),1);
	checkSuccess(cur->sendQuery("create or replace function sp_testtable(value in number) return types.cursortype is l_cursor    types.cursorType; begin open l_cursor for select * from testtable where testnumber>value; return l_cursor; end;"),1);
	cur->prepareQuery("begin  :curs1:=sp_testtable(5);  :curs2:=sp_testtable(0); end;");
	cur->defineOutputBindCursor("curs1");
	cur->defineOutputBindCursor("curs2");
	checkSuccess(cur->executeQuery(),1);
	sqlrcursor	*bindcur1=cur->getOutputBindCursor("curs1");
	checkSuccess(bindcur1->fetchFromBindCursor(),1);
	checkSuccess(bindcur1->getField(0,(uint32_t)0),"6");
	checkSuccess(bindcur1->getField(1,(uint32_t)0),"7");
	checkSuccess(bindcur1->getField(2,(uint32_t)0),"8");
	delete bindcur1;
	sqlrcursor	*bindcur2=cur->getOutputBindCursor("curs2");
	checkSuccess(bindcur2->fetchFromBindCursor(),1);
	checkSuccess(bindcur2->getField(0,(uint32_t)0),"1");
	checkSuccess(bindcur2->getField(1,(uint32_t)0),"2");
	checkSuccess(bindcur2->getField(2,(uint32_t)0),"3");
	delete bindcur2;
	checkSuccess(cur->sendQuery("drop package types"),1);
	stdoutput.printf("\n");

	stdoutput.printf("LONG CLOB: \n");
	cur->sendQuery("drop table testtable2");
	cur->sendQuery("create table testtable2 (testclob clob)");
	cur->prepareQuery("insert into testtable2 values (:clobval)");
	char	clobval[8*1024+1];
	for (int i=0; i<8*1024; i++) {
		clobval[i]='C';
	}
	clobval[8*1024]='\0';
	cur->inputBindClob("clobval",clobval,8*1024);
	checkSuccess(cur->executeQuery(),1);

	cur->sendQuery("select testclob from testtable2");
	checkSuccess(clobval,cur->getField(0,"TESTCLOB"));
	cur->prepareQuery("begin select testclob into :clobbindval from testtable2; end;");
	cur->defineOutputBindClob("clobbindval");
	checkSuccess(cur->executeQuery(),1);
	const char	*clobbindvar=cur->getOutputBindClob("clobbindval");
	checkSuccess(cur->getOutputBindLength("clobbindval"),8*1024);
	checkSuccess(clobval,clobbindvar);
	cur->sendQuery("drop table testtable2");
	stdoutput.printf("\n");


	stdoutput.printf("LONG OUTPUT BIND\n");
	cur->sendQuery("drop table testtable2");
	cur->sendQuery("create table testtable2 (testval varchar2(4000))");
	char	testval[4001];
	testval[4000]='\0';
	cur->prepareQuery("insert into testtable2 values (:testval)");
	for (int i=0; i<4000; i++) {
		testval[i]='C';
	}
	cur->inputBind("testval",testval);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testval from testtable2");
	checkSuccess(testval,cur->getField(0,"TESTVAL"));
	char	query[4000+25];
	charstring::printf(query,sizeof(query),
				"begin :bindval:='%s'; end;",testval);
	cur->prepareQuery(query);
	cur->defineOutputBindString("bindval",4000);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindLength("bindval"),4000);
	checkSuccess(cur->getOutputBindString("bindval"),testval);
	cur->sendQuery("drop table testtable2");
	stdoutput.printf("\n");

	stdoutput.printf("NEGATIVE INPUT BIND\n");
	cur->sendQuery("drop table testtable2");
	cur->sendQuery("create table testtable2 (testval number)");
	cur->prepareQuery("insert into testtable2 values (:testval)");
	cur->inputBind("testval",-1);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("select testval from testtable2");
	checkSuccess(cur->getField(0,"TESTVAL"),"-1");
	cur->sendQuery("drop table testtable2");
	stdoutput.printf("\n");



	stdoutput.printf("BIND VALIDATION: \n");
	cur->sendQuery("drop table testtable1");
	cur->sendQuery("create table testtable1 (col1 varchar2(20), col2 varchar2(20), col3 varchar2(20))");
	cur->prepareQuery("insert into testtable1 values ($(var1),$(var2),$(var3))");
	cur->inputBind("var1",1);
	cur->inputBind("var2",2);
	cur->inputBind("var3",3);
	cur->substitution("var1",":var1");
	checkSuccess(cur->validBind("var1"),1);
	checkSuccess(cur->validBind("var2"),0);
	checkSuccess(cur->validBind("var3"),0);
	checkSuccess(cur->validBind("var4"),0);
	stdoutput.printf("\n");
	cur->substitution("var2",":var2");
	checkSuccess(cur->validBind("var1"),1);
	checkSuccess(cur->validBind("var2"),1);
	checkSuccess(cur->validBind("var3"),0);
	checkSuccess(cur->validBind("var4"),0);
	stdoutput.printf("\n");
	cur->substitution("var3",":var3");
	checkSuccess(cur->validBind("var1"),1);
	checkSuccess(cur->validBind("var2"),1);
	checkSuccess(cur->validBind("var3"),1);
	checkSuccess(cur->validBind("var4"),0);
	checkSuccess(cur->executeQuery(),1);
	cur->sendQuery("drop table testtable1");
	stdoutput.printf("\n");

	// drop existing table
	cur->sendQuery("drop table testtable");


	// temporary tables
	stdoutput.printf("TEMPORARY TABLES: \n");
	cur->sendQuery("drop table temptabledelete\n");
	cur->sendQuery("create global temporary table temptabledelete (col1 number) on commit delete rows");
	checkSuccess(cur->sendQuery("insert into temptabledelete values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptabledelete"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(con->commit(),1);
	checkSuccess(cur->sendQuery("select count(*) from temptabledelete"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"0");
	cur->sendQuery("drop table temptabledelete\n");
	stdoutput.printf("\n");
	cur->sendQuery("truncate table temptablepreserve\n");
	cur->sendQuery("drop table temptablepreserve\n");
	cur->sendQuery("create global temporary table temptablepreserve (col1 number) on commit preserve rows");
	checkSuccess(cur->sendQuery("insert into temptablepreserve values (1)"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	checkSuccess(con->commit(),1);
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	con->endSession();
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"0");
	checkSuccess(cur->sendQuery("truncate table temptablepreserve\n"),1);
	checkSuccess(cur->sendQuery("drop table temptablepreserve\n"),1);
	checkSuccess(cur->sendQuery("select count(*) from temptablepreserve"),0);
	stdoutput.printf("\n");


	// stored procedures
	stdoutput.printf("STORED PROCEDURE: \n");
	// return no value
	cur->sendQuery("drop function testproc");
	cur->sendQuery("drop procedure testproc");
	checkSuccess(cur->sendQuery("create or replace procedure testproc(in1 in number, in2 in number, in3 in varchar2) is begin return; end;"),1);
	cur->prepareQuery("begin testproc(:in1,:in2,:in3); end;");
	cur->inputBind("in1",1);
	cur->inputBind("in2",1.1,2,1);
	cur->inputBind("in3","hello");
	checkSuccess(cur->executeQuery(),1);
	// return single value
	cur->sendQuery("drop function testproc");
	cur->sendQuery("drop procedure testproc");
	checkSuccess(cur->sendQuery("create or replace function testproc(in1 in number, in2 in number, in3 in varchar2) return number is begin return in1; end;"),1);
	cur->prepareQuery("select testproc(:in1,:in2,:in3) from dual");
	cur->inputBind("in1",1);
	cur->inputBind("in2",1.1,2,1);
	cur->inputBind("in3","hello");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getField(0,(uint32_t)0),"1");
	cur->prepareQuery("begin  :out1:=testproc(:in1,:in2,:in3); end;");
	cur->inputBind("in1",1);
	cur->inputBind("in2",1.1,2,1);
	cur->inputBind("in3","hello");
	cur->defineOutputBindInteger("out1");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindInteger("out1"),1);
	// return multiple values
	cur->sendQuery("drop function testproc");
	cur->sendQuery("drop procedure testproc");
	checkSuccess(cur->sendQuery("create or replace procedure testproc(in1 in number, in2 in number, in3 in varchar2, out1 out number, out2 out number, out3 out varchar2) is begin out1:=in1; out2:=in2; out3:=in3; end;"),1);
	cur->prepareQuery("begin testproc(:in1,:in2,:in3,:out1,:out2,:out3); end;");
	cur->inputBind("in1",1);
	cur->inputBind("in2",1.1,2,1);
	cur->inputBind("in3","hello");
	cur->defineOutputBindInteger("out1");
	cur->defineOutputBindDouble("out2");
	cur->defineOutputBindString("out3",20);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindInteger("out1"),1);
	checkSuccess(cur->getOutputBindDouble("out2"),1.1);
	checkSuccess(cur->getOutputBindString("out3"),"hello");
	cur->sendQuery("drop function testproc");
	cur->sendQuery("drop procedure testproc");
	stdoutput.printf("\n");


	// in/out variables
	/*stdoutput.printf("IN/OUT VARIABLES: \n");
	cur->sendQuery("drop procedure testproc");
	checkSuccess(cur->sendQuery("create or replace procedure testproc(inout in out number) is begin inout:=inout+1; return; end;"),1);
	cur->prepareQuery("begin testproc(:inout); end;");
	cur->inputBind("inout",1);
	cur->defineOutputBindInteger("inout");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindInteger("inout"),2);
	cur->sendQuery("drop procedure testproc");
	stdoutput.printf("\n");*/



	// rebinding
	stdoutput.printf("REBINDING: \n");
	cur->sendQuery("drop procedure testproc");
	checkSuccess(cur->sendQuery("create or replace procedure testproc(in1 in number, out1 out number) is begin out1:=in1; return; end;"),1);
	cur->prepareQuery("begin testproc(:in,:out); end;");
	cur->inputBind("in",1);
	cur->defineOutputBindInteger("out");
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindInteger("out"),1);
	cur->inputBind("in",2);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindInteger("out"),2);
	cur->inputBind("in",3);
	checkSuccess(cur->executeQuery(),1);
	checkSuccess(cur->getOutputBindInteger("out"),3);
	cur->sendQuery("drop procedure testproc");
	stdoutput.printf("\n");


	// invalid queries...
	stdoutput.printf("INVALID QUERIES: \n");
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),0);
	checkSuccess(cur->sendQuery("select * from testtable order by testnumber"),0);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	checkSuccess(cur->sendQuery("insert into testtable values (1,2,3,4)"),0);
	stdoutput.printf("\n");
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	checkSuccess(cur->sendQuery("create table testtable"),0);
	stdoutput.printf("\n");


	delete cur;
	delete con;

	return 0;
}
