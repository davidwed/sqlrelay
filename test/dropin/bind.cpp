#include <mysql.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/bytestring.h>
#include <rudiments/environment.h>
#include <rudiments/stdio.h>
#include <config.h>

//#define FULL 1

MYSQL		mysql;
MYSQL_RES	*result;
MYSQL_FIELD	*field;
MYSQL_ROW	row;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
			stdoutput.printf("failure ");
stdoutput.printf("\n%s\n",mysql_error(&mysql));
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
		stdoutput.printf("failure ");
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%d\"!=\"%d\"\n",value,success);
		stdoutput.printf("failure ");
stdoutput.printf("\n%s\n",mysql_error(&mysql));
		process::exit(1);
	}
}

int	main(int argc, char **argv) {

	const char	*host;
	const char	*port;
	const char	*socket;
	const char	*user;
	const char	*password;
	const char	*db;
	if (!charstring::isNullOrEmpty(environment::getValue("LD_PRELOAD"))) {
		host="127.0.0.1";
		port="9000";
		socket="/tmp/test.socket";
		user="test";
		password="test";
		db="";
	} else {
		// to run against a real mysql instance, provide a host name
		// eg: ./mysql db64
		if (argc==2) {
			host=argv[1];
		} else {
			host="127.0.0.1";
		}
#ifdef FULL
		db="testdb";
		port="3306";
		socket="/var/lib/mysql/mysql.sock";
		user="testuser";
		password="testpassword";
#else
		db="dbtest_enc";
		port="6033";
		socket="/var/lib/mysql/mysql.sock";
		user="secured";
		password="pwd4secured";
#endif
	}


	stdoutput.printf("\n============ Traditional API ============\n\n");

	#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
		stdoutput.printf("mysql_init\n");
		checkSuccess((long)mysql_init(&mysql),(long)&mysql);
		stdoutput.printf("\n");
		stdoutput.printf("mysql_real_connect\n");
		#if MYSQL_VERSION_ID>=32200
			checkSuccess((long)mysql_real_connect(
						&mysql,host,user,password,db,
						charstring::toInteger(port),
						socket,0),(long)&mysql);
		#else
			checkSuccess((long)mysql_real_connect(
						&mysql,host,user,password,
						charstring::toInteger(port),
						socket,0),(long)&mysql);
			if (!charstring::isNullOrEmpty(db)) {
				checkSuccess(mysql_select_db(&mysql,db),0);
			}
		#endif
	#else
		checkSuccess((long)mysql_connect(&mysql,host,
						user,password),
						(long)mysql);
	#endif
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_init:\n");
	MYSQL_STMT	*stmt=mysql_stmt_init(&mysql);

	stdoutput.printf("mysql_stmt_prepare\n");
#ifdef FULL
	const char	*query="select ?,?,?,?,?,?,?,?,?,?,?,?,?,?";
#else
	const char	*query="select ?,?";
#endif
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
#ifdef FULL
	MYSQL_BIND	bind[14];
	unsigned long	bindlength[14];
	my_bool		bindisnull[14];
#else
	MYSQL_BIND	bind[2];
	unsigned long	bindlength[2];
	my_bool		bindisnull[2];
#endif
	bytestring::zero(&bind,sizeof(bind));
	bytestring::zero(&bindlength,sizeof(bindlength));
	bytestring::zero(&bindisnull,sizeof(bindisnull));

#ifdef FULL
	char	tinyval=1;
	bind[0].buffer_type=MYSQL_TYPE_TINY;
	bind[0].buffer=&tinyval;
	bind[0].buffer_length=sizeof(tinyval);
	bindlength[0]=sizeof(tinyval);
	bind[0].length=&bindlength[0];
	bindisnull[0]=0;
	bind[0].is_null=&bindisnull[0];

	int16_t	shortval=1;
	bind[1].buffer_type=MYSQL_TYPE_SHORT;
	bind[1].buffer=&shortval;
	bind[1].buffer_length=sizeof(shortval);
	bindlength[1]=sizeof(shortval);
	bind[1].length=&bindlength[1];
	bindisnull[1]=0;
	bind[1].is_null=&bindisnull[1];

	int32_t	longval=1;
	bind[2].buffer_type=MYSQL_TYPE_LONG;
	bind[2].buffer=&longval;
	bind[2].buffer_length=sizeof(longval);
	bindlength[2]=sizeof(longval);
	bind[2].length=&bindlength[2];
	bindisnull[2]=0;
	bind[2].is_null=&bindisnull[2];

	int64_t	longlongval=1;
	bind[3].buffer_type=MYSQL_TYPE_LONGLONG;
	bind[3].buffer=&longlongval;
	bind[3].buffer_length=sizeof(longlongval);
	bindlength[3]=sizeof(longlongval);
	bind[3].length=&bindlength[3];
	bindisnull[3]=0;
	bind[3].is_null=&bindisnull[3];

	float	floatval=1.1;
	bind[4].buffer_type=MYSQL_TYPE_FLOAT;
	bind[4].buffer=&floatval;
	bind[4].buffer_length=sizeof(floatval);
	bindlength[4]=sizeof(floatval);
	bind[4].length=&bindlength[4];
	bindisnull[4]=0;
	bind[4].is_null=&bindisnull[4];

	double	doubleval=1.1;
	bind[5].buffer_type=MYSQL_TYPE_DOUBLE;
	bind[5].buffer=&doubleval;
	bind[5].buffer_length=sizeof(doubleval);
	bindlength[5]=sizeof(doubleval);
	bind[5].length=&bindlength[5];
	bindisnull[5]=0;
	bind[5].is_null=&bindisnull[5];

	bind[6].buffer_type=MYSQL_TYPE_STRING;
	bind[6].buffer=(void *)"string1";
	bind[6].buffer_length=7;
	bindlength[6]=7;
	bind[6].length=&bindlength[6];
	bindisnull[6]=0;
	bind[6].is_null=&bindisnull[6];

	bind[7].buffer_type=MYSQL_TYPE_VAR_STRING;
	bind[7].buffer=(void *)"varstring1";
	bind[7].buffer_length=10;
	bindlength[7]=10;
	bind[7].length=&bindlength[7];
	bindisnull[7]=0;
	bind[7].is_null=&bindisnull[7];

	bind[8].buffer_type=MYSQL_TYPE_DATE;
	MYSQL_TIME	datetm;
	bytestring::zero(&datetm,sizeof(datetm));
	datetm.year=2001;
	datetm.month=1;
	datetm.day=2;
	bind[8].buffer=(void *)&datetm;
	bind[8].buffer_length=sizeof(datetm);
	bindlength[8]=sizeof(datetm);
	bind[8].length=&bindlength[8];
	bindisnull[8]=0;
	bind[8].is_null=&bindisnull[8];

	bind[9].buffer_type=MYSQL_TYPE_TIME;
	MYSQL_TIME	timetm;
	bytestring::zero(&timetm,sizeof(timetm));
	timetm.neg=1;
	timetm.day=1;
	timetm.hour=12;
	timetm.minute=10;
	timetm.second=11;
	bind[9].buffer=(void *)&timetm;
	bind[9].buffer_length=sizeof(timetm);
	bindlength[9]=sizeof(timetm);
	bind[9].length=&bindlength[9];
	bindisnull[9]=0;
	bind[9].is_null=&bindisnull[9];

	bind[10].buffer_type=MYSQL_TYPE_DATETIME;
	MYSQL_TIME	datetimetm;
	bytestring::zero(&datetimetm,sizeof(datetimetm));
	datetimetm.year=2001;
	datetimetm.month=1;
	datetimetm.day=2;
	datetimetm.hour=12;
	datetimetm.minute=10;
	datetimetm.second=11;
	bind[10].buffer=(void *)&datetimetm;
	bind[10].buffer_length=sizeof(datetimetm);
	bindlength[10]=sizeof(datetimetm);
	bind[10].length=&bindlength[10];
	bindisnull[10]=0;
	bind[10].is_null=&bindisnull[10];

	bind[11].buffer_type=MYSQL_TYPE_TINY_BLOB;
	bind[11].buffer=(void *)"tinyblob1";
	bind[11].buffer_length=9;
	bindlength[11]=9;
	bind[11].length=&bindlength[11];
	bindisnull[11]=0;
	bind[11].is_null=&bindisnull[11];

	bind[12].buffer_type=MYSQL_TYPE_MEDIUM_BLOB;
	bind[12].buffer=(void *)"mediumblob1";
	bind[12].buffer_length=11;
	bindlength[12]=11;
	bind[12].length=&bindlength[12];
	bindisnull[12]=0;
	bind[12].is_null=&bindisnull[12];

	bind[13].buffer_type=MYSQL_TYPE_LONG_BLOB;
	bind[13].buffer=(void *)"longblob1";
	bind[13].buffer_length=9;
	bindlength[13]=9;
	bind[13].length=&bindlength[13];
	bindisnull[13]=0;
	bind[13].is_null=&bindisnull[13];
#else
	char	tinyval=1;
	bind[0].buffer_type=MYSQL_TYPE_TINY;
	bind[0].buffer=&tinyval;
	bind[0].buffer_length=sizeof(tinyval);
	bindlength[0]=sizeof(tinyval);
	bind[0].length=&bindlength[0];
	bindisnull[0]=0;
	bind[0].is_null=&bindisnull[0];

	bind[1].buffer_type=MYSQL_TYPE_STRING;
	bind[1].buffer=(void *)"string1";
	bind[1].buffer_length=7;
	bindlength[1]=7;
	bind[1].length=&bindlength[1];
	bindisnull[1]=0;
	bind[1].is_null=&bindisnull[1];
#endif

	checkSuccess(mysql_stmt_bind_param(stmt,bind),0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_execute\n");
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	#ifdef MARIADB_BASE_VERSION
	checkSuccess(mysql_stmt_fetch(stmt),100);
	#endif
	stdoutput.printf("\n");
	stdoutput.printf("mysql_stmt_execute\n");
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	#ifdef MARIADB_BASE_VERSION
	checkSuccess(mysql_stmt_fetch(stmt),100);
	#endif
	stdoutput.printf("\n");
	stdoutput.printf("mysql_stmt_execute\n");
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	#ifdef MARIADB_BASE_VERSION
	checkSuccess(mysql_stmt_fetch(stmt),100);
	#endif
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_close:\n");
	checkSuccess(mysql_stmt_close(stmt),0);
	stdoutput.printf("\n");


	mysql_close(&mysql);
}
