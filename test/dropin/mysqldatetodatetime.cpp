#include <mysql.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/bytestring.h>
#include <rudiments/environment.h>
#include <rudiments/stdio.h>
#include <config.h>

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
		db="testdb";
		port="3306";
		socket="/var/lib/mysql/mysql.sock";
		user="testuser";
		password="testpassword";
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

	const char	*query="drop table testtable";
	mysql_real_query(&mysql,query,charstring::length(query));

	stdoutput.printf("create\n");
	query="create table testtable (col1 date)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	stdoutput.printf("\n");

	stdoutput.printf("list fields\n");
	result=mysql_list_fields(&mysql,"testtable",NULL);
	field=mysql_fetch_field_direct(result,0);
	checkSuccess(field->type,MYSQL_TYPE_DATETIME);
	stdoutput.printf("\n");

	stdoutput.printf("alter nls_date_format\n");
	query="alter session set nls_date_format='YYYY-MM-DD HH24:MI:SS'";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);

	stdoutput.printf("insert\n");
	query="insert into testtable values ('2001-01-01 01:00:00')";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	stdoutput.printf("\n");

	stdoutput.printf("select\n");
	query="select * from testtable";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	field=mysql_fetch_field_direct(result,0);
	checkSuccess(field->type,MYSQL_TYPE_DATETIME);
	stdoutput.printf("\n");

	stdoutput.printf("drop\n");
	query="drop table testtable";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_info(&mysql),NULL);
	stdoutput.printf("\n");

	mysql_close(&mysql);
}
