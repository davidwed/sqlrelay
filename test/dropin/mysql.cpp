#include <mysql.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/bytestring.h>
#include <rudiments/environment.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/stdio.h>
#include <config.h>

// MySQL 8+ doesn't have my_bool, but MariaDB 10+ does
#ifndef MARIADB_BASE_VERSION
	#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID>=80000
		typedef bool my_bool;
	#endif
#endif

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

	#ifdef HAVE_MYSQL_PING
	stdoutput.printf("mysql_ping\n");
	checkSuccess(mysql_ping(&mysql),0);
	stdoutput.printf("\n");
	#endif

	stdoutput.printf("mysql_character_set_name:\n");
	#if !defined(MARIADB_BASE_VERSION) && \
		defined(MYSQL_VERSION_ID) && \
		MYSQL_VERSION_ID>=80000
	checkSuccess((char *)mysql_character_set_name(&mysql),"utf8mb4");
	#else
	const char	*charset=mysql_character_set_name(&mysql);
	checkSuccess(!charstring::compare(charset,"latin1") ||
				!charstring::compare(charset,"utf8"),true);
	#endif
	stdoutput.printf("\n");

	stdoutput.printf("mysql_list_dbs\n");
	result=mysql_list_dbs(&mysql,NULL);
	// FIXME: crashes with drop-in lib
	//checkSuccess(mysql_field_count(&mysql),1);
	checkSuccess(mysql_num_fields(result),1);
	field=mysql_fetch_field_direct(result,0);
	if (charstring::isNullOrEmpty(environment::getValue("LD_PRELOAD"))) {
		checkSuccess(
			!charstring::compare(field->name,"Database") || 
			!charstring::compare(field->name,"Database (%)"),1);
	} else {
		// sqlrelay calls this column schema_name rather
		// than Database so the drop-in lib does too
		checkSuccess(field->name,"schema_name");
	}
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"information_schema");
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"testdb");
	row=mysql_fetch_row(result);
	checkSuccess((row==NULL),1);
	mysql_free_result(result);
	stdoutput.printf("\n");

	const char	*query="drop table testdb.testtable";
	mysql_real_query(&mysql,query,charstring::length(query));

	stdoutput.printf("mysql_real_query: create\n");
	query="create table testdb.testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(2,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testtext text, testvarchar varchar(40), testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testtimestamp timestamp)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_info(&mysql),NULL);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_list_tables\n");
	result=mysql_list_tables(&mysql,NULL);
	// FIXME: crashes with drop-in lib
	//checkSuccess(mysql_field_count(&mysql),1);
	checkSuccess(mysql_num_fields(result),1);
	field=mysql_fetch_field_direct(result,0);
	if (charstring::isNullOrEmpty(environment::getValue("LD_PRELOAD"))) {
		checkSuccess(
		!charstring::compare(field->name,"Tables_in_testdb") || 
		!charstring::compare(field->name,"Tables_in_testdb (%)"),1);
	} else {
		// sqlrelay calls this column schema_name rather
		// than Database so the drop-in lib does too
		checkSuccess(field->name,"table_name");
	}
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"testtable");
	row=mysql_fetch_row(result);
	checkSuccess((row==NULL),1);
	mysql_free_result(result);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_list_fields\n");
	result=mysql_list_fields(&mysql,"testtable",NULL);
	// FIXME: crashes with drop-in lib
	//checkSuccess(mysql_field_count(&mysql),19);
	checkSuccess(mysql_num_fields(result),19);
	stdoutput.printf("\n");

	stdoutput.printf("tinyint\n");
	field=mysql_fetch_field_direct(result,0);
	checkSuccess(field->name,"testtinyint");
	/*if (argc==2) {
		checkSuccess(field->org_name,"testtinyint");
		checkSuccess(field->table,"testtable");
		checkSuccess(field->org_table,"testtable");
		checkSuccess(field->db,"testdb");
	}*/
	checkSuccess(field->catalog,"def");
	checkSuccess(field->def,NULL);
	checkSuccess(field->length,4);
	checkSuccess(field->max_length,0);
	checkSuccess(field->name_length,11);
	/*if (argc==2) {
		checkSuccess(field->org_name_length,11);
		checkSuccess(field->db_length,6);
	}*/
	checkSuccess(field->catalog_length,3);
	// Some client API's don't set this if def is NULL
	//checkSuccess(field->def_length,0);
	checkSuccess(field->flags,NUM_FLAG);
	checkSuccess(field->decimals,0);
	/*if (argc==2) {
		checkSuccess(field->charsetnr,63);
	}*/
	checkSuccess(field->type,MYSQL_TYPE_TINY);
	stdoutput.printf("\n");

	stdoutput.printf("smallint\n");
	field=mysql_fetch_field_direct(result,1);
	checkSuccess(field->name,"testsmallint");
	checkSuccess(field->length,6);
	checkSuccess(field->flags,NUM_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_SHORT);
	stdoutput.printf("\n");

	stdoutput.printf("mediumint\n");
	field=mysql_fetch_field_direct(result,2);
	checkSuccess(field->name,"testmediumint");
	checkSuccess(field->length,9);
	checkSuccess(field->flags,NUM_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_INT24);
	stdoutput.printf("\n");

	stdoutput.printf("int\n");
	field=mysql_fetch_field_direct(result,3);
	checkSuccess(field->name,"testint");
	checkSuccess(field->length,11);
	checkSuccess(field->flags,NUM_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_LONG);
	stdoutput.printf("\n");

	stdoutput.printf("bigint\n");
	field=mysql_fetch_field_direct(result,4);
	checkSuccess(field->name,"testbigint");
	checkSuccess(field->length,20);
	checkSuccess(field->flags,NUM_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_LONGLONG);
	stdoutput.printf("\n");

	stdoutput.printf("float\n");
	field=mysql_fetch_field_direct(result,5);
	checkSuccess(field->name,"testfloat");
	checkSuccess(field->length,12);
	checkSuccess(field->flags,NUM_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_FLOAT);
	stdoutput.printf("\n");

	stdoutput.printf("real\n");
	field=mysql_fetch_field_direct(result,6);
	checkSuccess(field->name,"testreal");
	checkSuccess(field->length,22);
	checkSuccess(field->flags,NUM_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_DOUBLE);
	stdoutput.printf("\n");

	stdoutput.printf("decimal\n");
	field=mysql_fetch_field_direct(result,7);
	checkSuccess(field->name,"testdecimal");
	checkSuccess(field->length,4);
	// MariaDB LGPL connector doesn't always get this right
	//checkSuccess(field->flags,NUM_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_NEWDECIMAL);
	checkSuccess(field->decimals,1);
	stdoutput.printf("\n");

	stdoutput.printf("date\n");
	field=mysql_fetch_field_direct(result,8);
	checkSuccess(field->name,"testdate");
	checkSuccess(field->length,10);
	checkSuccess(field->flags,BINARY_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_DATE);
	stdoutput.printf("\n");

	stdoutput.printf("time\n");
	field=mysql_fetch_field_direct(result,9);
	checkSuccess(field->name,"testtime");
	checkSuccess(field->length,10);
	checkSuccess(field->flags,BINARY_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_TIME);
	stdoutput.printf("\n");

	stdoutput.printf("datetime\n");
	field=mysql_fetch_field_direct(result,10);
	checkSuccess(field->name,"testdatetime");
	checkSuccess(field->length,19);
	checkSuccess(field->flags,BINARY_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_DATETIME);
	stdoutput.printf("\n");

	stdoutput.printf("year\n");
	field=mysql_fetch_field_direct(result,11);
	checkSuccess(field->name,"testyear");
	checkSuccess(field->length,4);
	checkSuccess(field->flags,NUM_FLAG|UNSIGNED_FLAG|ZEROFILL_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_YEAR);
	stdoutput.printf("\n");

	stdoutput.printf("char\n");
	field=mysql_fetch_field_direct(result,12);
	checkSuccess(field->name,"testchar");
	checkSuccess(field->length,40);
	checkSuccess(field->flags,0);
	checkSuccess(field->type,MYSQL_TYPE_STRING);
	stdoutput.printf("\n");

	stdoutput.printf("text\n");
	field=mysql_fetch_field_direct(result,13);
	checkSuccess(field->name,"testtext");
	checkSuccess(field->length,65535);
	checkSuccess(field->flags,BLOB_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_BLOB);
	stdoutput.printf("\n");

	stdoutput.printf("varchar\n");
	field=mysql_fetch_field_direct(result,14);
	checkSuccess(field->name,"testvarchar");
	checkSuccess(field->length,40);
	checkSuccess(field->flags,0);
	checkSuccess(field->type,MYSQL_TYPE_VAR_STRING);
	stdoutput.printf("\n");

	stdoutput.printf("tinytext\n");
	field=mysql_fetch_field_direct(result,15);
	checkSuccess(field->name,"testtinytext");
	checkSuccess(field->length,255);
	checkSuccess(field->flags,BLOB_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_BLOB);
	stdoutput.printf("\n");

	stdoutput.printf("mediumtext\n");
	field=mysql_fetch_field_direct(result,16);
	checkSuccess(field->name,"testmediumtext");
	checkSuccess(field->length,16777215);
	checkSuccess(field->flags,BLOB_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_BLOB);
	stdoutput.printf("\n");

	stdoutput.printf("longtext\n");
	field=mysql_fetch_field_direct(result,17);
	checkSuccess(field->name,"testlongtext");
	// The length of these can be reported by the db as either
	// 2^31-1 or 2^32-1.  It's not clear why it varies, but it can.
	//checkSuccess(field->length,2147483647);
	checkSuccess(field->flags,BLOB_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_BLOB);
	stdoutput.printf("\n");

	stdoutput.printf("timestamp\n");
	field=mysql_fetch_field_direct(result,18);
	checkSuccess(field->name,"testtimestamp");
	checkSuccess(field->length,19);
	checkSuccess(field->flags,
			TIMESTAMP_FLAG|ON_UPDATE_NOW_FLAG|
			BINARY_FLAG|UNSIGNED_FLAG|NOT_NULL_FLAG);
	checkSuccess(field->type,MYSQL_TYPE_TIMESTAMP);
	mysql_free_result(result);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_real_query: insert\n");
	query="insert into testdb.testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','text1','varchar1','tinytext1','mediumtext1','longtext1',NULL)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_info(&mysql),NULL);
	checkSuccess(mysql_affected_rows(&mysql),1);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_send_query: insert\n");
	query="insert into testdb.testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','text2','varchar2','tinytext2','mediumtext2','longtext2',NULL)";
	checkSuccess(mysql_send_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_info(&mysql),NULL);
	checkSuccess(mysql_read_query_result(&mysql),0);
	checkSuccess(mysql_affected_rows(&mysql),1);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_real_query: select\n");
	query="select * from testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_info(&mysql),NULL);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_field_count:\n");
	checkSuccess(mysql_field_count(&mysql),19);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_store_result:\n");
	result=mysql_store_result(&mysql);

	stdoutput.printf("mysql_num_fields:\n");
	checkSuccess(mysql_num_fields(result),19);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_num_rows:\n");
	checkSuccess(mysql_num_rows(result),2);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_field_seek:\n");
	checkSuccess(mysql_field_seek(result,0),0);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_fetch_field/mysql_field_tell:\n");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),1);
	checkSuccess(field->name,"testtinyint");
	// FIXME: field->...
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),2);
	checkSuccess(field->name,"testsmallint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),3);
	checkSuccess(field->name,"testmediumint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),4);
	checkSuccess(field->name,"testint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),5);
	checkSuccess(field->name,"testbigint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),6);
	checkSuccess(field->name,"testfloat");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),7);
	checkSuccess(field->name,"testreal");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),8);
	checkSuccess(field->name,"testdecimal");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),9);
	checkSuccess(field->name,"testdate");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),10);
	checkSuccess(field->name,"testtime");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),11);
	checkSuccess(field->name,"testdatetime");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),12);
	checkSuccess(field->name,"testyear");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),13);
	checkSuccess(field->name,"testchar");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),14);
	checkSuccess(field->name,"testtext");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),15);
	checkSuccess(field->name,"testvarchar");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),16);
	checkSuccess(field->name,"testtinytext");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),17);
	checkSuccess(field->name,"testmediumtext");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),18);
	checkSuccess(field->name,"testlongtext");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),19);
	checkSuccess(field->name,"testtimestamp");
	stdoutput.printf("\n");


	stdoutput.printf("mysql_field_seek:\n");
	checkSuccess(mysql_field_seek(result,0),19);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_fetch_field_direct:\n");
	field=mysql_fetch_field_direct(result,0);
	checkSuccess(field->name,"testtinyint");
	// FIXME: field->...
	field=mysql_fetch_field_direct(result,1);
	checkSuccess(field->name,"testsmallint");
	field=mysql_fetch_field_direct(result,2);
	checkSuccess(field->name,"testmediumint");
	field=mysql_fetch_field_direct(result,3);
	checkSuccess(field->name,"testint");
	field=mysql_fetch_field_direct(result,4);
	checkSuccess(field->name,"testbigint");
	field=mysql_fetch_field_direct(result,5);
	checkSuccess(field->name,"testfloat");
	field=mysql_fetch_field_direct(result,6);
	checkSuccess(field->name,"testreal");
	field=mysql_fetch_field_direct(result,7);
	checkSuccess(field->name,"testdecimal");
	field=mysql_fetch_field_direct(result,8);
	checkSuccess(field->name,"testdate");
	field=mysql_fetch_field_direct(result,9);
	checkSuccess(field->name,"testtime");
	field=mysql_fetch_field_direct(result,10);
	checkSuccess(field->name,"testdatetime");
	field=mysql_fetch_field_direct(result,11);
	checkSuccess(field->name,"testyear");
	field=mysql_fetch_field_direct(result,12);
	checkSuccess(field->name,"testchar");
	field=mysql_fetch_field_direct(result,13);
	checkSuccess(field->name,"testtext");
	field=mysql_fetch_field_direct(result,14);
	checkSuccess(field->name,"testvarchar");
	field=mysql_fetch_field_direct(result,15);
	checkSuccess(field->name,"testtinytext");
	field=mysql_fetch_field_direct(result,16);
	checkSuccess(field->name,"testmediumtext");
	field=mysql_fetch_field_direct(result,17);
	checkSuccess(field->name,"testlongtext");
	field=mysql_fetch_field_direct(result,18);
	checkSuccess(field->name,"testtimestamp");
	stdoutput.printf("\n");


	stdoutput.printf("mysql_fetch_fields:\n");
	field=mysql_fetch_fields(result);
	checkSuccess(field[0].name,"testtinyint");
	checkSuccess(field[1].name,"testsmallint");
	checkSuccess(field[2].name,"testmediumint");
	checkSuccess(field[3].name,"testint");
	checkSuccess(field[4].name,"testbigint");
	checkSuccess(field[5].name,"testfloat");
	checkSuccess(field[6].name,"testreal");
	checkSuccess(field[7].name,"testdecimal");
	checkSuccess(field[8].name,"testdate");
	checkSuccess(field[9].name,"testtime");
	checkSuccess(field[10].name,"testdatetime");
	checkSuccess(field[11].name,"testyear");
	checkSuccess(field[12].name,"testchar");
	checkSuccess(field[13].name,"testtext");
	checkSuccess(field[14].name,"testvarchar");
	checkSuccess(field[15].name,"testtinytext");
	checkSuccess(field[16].name,"testmediumtext");
	checkSuccess(field[17].name,"testlongtext");
	checkSuccess(field[18].name,"testtimestamp");
	stdoutput.printf("\n");


	stdoutput.printf("mysql_fetch_row:\n");
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"1");
	checkSuccess(row[1],"1");
	checkSuccess(row[2],"1");
	checkSuccess(row[3],"1");
	checkSuccess(row[4],"1");
	//checkSuccess(row[5],"1.1");
	//checkSuccess(row[6],"1.1");
	checkSuccess(row[7],"1.1");
	checkSuccess(row[8],"2001-01-01");
	checkSuccess(row[9],"01:00:00");
	checkSuccess(row[10],"2001-01-01 01:00:00");
	checkSuccess(row[11],"2001");
	checkSuccess(row[12],"char1");
	checkSuccess(!charstring::compare(row[13],"text1",5),1);
	checkSuccess(row[14],"varchar1");
	checkSuccess(!charstring::compare(row[15],"tinytext1",9),1);
	checkSuccess(!charstring::compare(row[16],"mediumtext1",11),1);
	checkSuccess(!charstring::compare(row[17],"longtext1",9),1);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_fetch_lengths:\n");
	unsigned long	*lengths;
	lengths=mysql_fetch_lengths(result);
	checkSuccess(lengths[0],1);
	checkSuccess(lengths[1],1);
	checkSuccess(lengths[2],1);
	checkSuccess(lengths[3],1);
	checkSuccess(lengths[4],1);
	//checkSuccess(lengths[5],3);
	//checkSuccess(lengths[6],3);
	checkSuccess(lengths[7],3);
	checkSuccess(lengths[8],10);
	checkSuccess(lengths[9],8);
	checkSuccess(lengths[10],19);
	checkSuccess(lengths[11],4);
	checkSuccess(lengths[12],5);
	checkSuccess(lengths[13],5);
	checkSuccess(lengths[14],8);
	checkSuccess(lengths[15],9);
	checkSuccess(lengths[16],11);
	checkSuccess(lengths[17],9);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_fetch_row:\n");
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"2");
	checkSuccess(row[1],"2");
	checkSuccess(row[2],"2");
	checkSuccess(row[3],"2");
	checkSuccess(row[4],"2");
	//checkSuccess(row[5],"2.1");
	//checkSuccess(row[6],"2.1");
	checkSuccess(row[7],"2.1");
	checkSuccess(row[8],"2002-01-01");
	checkSuccess(row[9],"02:00:00");
	checkSuccess(row[10],"2002-01-01 02:00:00");
	checkSuccess(row[11],"2002");
	checkSuccess(row[12],"char2");
	checkSuccess(!charstring::compare(row[13],"text2",5),1);
	checkSuccess(row[14],"varchar2");
	checkSuccess(!charstring::compare(row[15],"tinytext2",9),1);
	checkSuccess(!charstring::compare(row[16],"mediumtext2",11),1);
	checkSuccess(!charstring::compare(row[17],"longtext2",9),1);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_data_seek:\n");
	mysql_data_seek(result,0);
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"1");
	stdoutput.printf("\n");


	stdoutput.printf("mysql_row_tell/mysql_row_seek:\n");
	mysql_data_seek(result,0);
	MYSQL_ROW_OFFSET	zerorowoffset=mysql_row_tell(result);
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"1");
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"2");
	mysql_row_seek(result,zerorowoffset);
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"1");
	stdoutput.printf("\n");


	stdoutput.printf("mysql_eof:\n");
	mysql_data_seek(result,1);
	row=mysql_fetch_row(result);
	checkSuccess(mysql_eof(result),1);
	stdoutput.printf("\n");


	mysql_free_result(result);



	stdoutput.printf("mysql_real_query: select\n");
	query="select * from testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_info(&mysql),NULL);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_use_result:\n");
	result=mysql_use_result(&mysql);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_fetch_row:\n");
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"1");
	checkSuccess(row[1],"1");
	checkSuccess(row[2],"1");
	checkSuccess(row[3],"1");
	checkSuccess(row[4],"1");
	//checkSuccess(row[5],"1.1");
	//checkSuccess(row[6],"1.1");
	checkSuccess(row[7],"1.1");
	checkSuccess(row[8],"2001-01-01");
	checkSuccess(row[9],"01:00:00");
	checkSuccess(row[10],"2001-01-01 01:00:00");
	checkSuccess(row[11],"2001");
	checkSuccess(row[12],"char1");
	checkSuccess(!charstring::compare(row[13],"text1",5),1);
	checkSuccess(row[14],"varchar1");
	checkSuccess(!charstring::compare(row[15],"tinytext1",9),1);
	checkSuccess(!charstring::compare(row[16],"mediumtext1",11),1);
	checkSuccess(!charstring::compare(row[17],"longtext1",9),1);
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"2");
	checkSuccess(row[1],"2");
	checkSuccess(row[2],"2");
	checkSuccess(row[3],"2");
	checkSuccess(row[4],"2");
	//checkSuccess(row[5],"2.1");
	//checkSuccess(row[6],"2.1");
	checkSuccess(row[7],"2.1");
	checkSuccess(row[8],"2002-01-01");
	checkSuccess(row[9],"02:00:00");
	checkSuccess(row[10],"2002-01-01 02:00:00");
	checkSuccess(row[11],"2002");
	checkSuccess(row[12],"char2");
	checkSuccess(!charstring::compare(row[13],"text2",5),1);
	checkSuccess(row[14],"varchar2");
	checkSuccess(!charstring::compare(row[15],"tinytext2",9),1);
	checkSuccess(!charstring::compare(row[16],"mediumtext2",11),1);
	checkSuccess(!charstring::compare(row[17],"longtext2",9),1);
	checkSuccess((long)mysql_fetch_row(result),0);
	stdoutput.printf("\n");


	mysql_free_result(result);


	stdoutput.printf("mysql_real_query: drop\n");
	query="drop table testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_info(&mysql),NULL);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_escape_string:\n");
	char	to[100];
	char	from[100];
	charstring::printf(from,sizeof(from)," ' \" \n \r \\ ; %c ",26);
	checkSuccess(mysql_escape_string(to,from,15),21);
	checkSuccess(to," \\' \\\" \\n \\r \\\\ ; \\Z ");
	stdoutput.printf("\n");


	stdoutput.printf("mysql_real_escape_string:\n");
	checkSuccess(mysql_real_escape_string(&mysql,to,from,15),21);
	checkSuccess(to," \\' \\\" \\n \\r \\\\ ; \\Z ");
	stdoutput.printf("\n");
	
	stdoutput.printf("mysql_insert_id\n");
	query="create table testdb.testtable (col1 int not null primary key auto_increment, col2 int)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	query="insert into testdb.testtable (col2) values (1)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_insert_id(&mysql),1);
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_insert_id(&mysql),2);
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_insert_id(&mysql),3);
	query="drop table testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	stdoutput.printf("\n");

	
	stdoutput.printf("mysql_error/mysql_errno\n");
	query="known bad query";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),1);
	char	*error=charstring::duplicate(mysql_error(&mysql));
	if (charstring::length(error)>36) {
		error[36]='\0';
	}
	checkSuccess(error,"You have an error in your SQL syntax");
	delete[] error;
	checkSuccess(mysql_errno(&mysql),1064);
	stdoutput.printf("\n");


	if (charstring::isNullOrEmpty(environment::getValue("LD_PRELOAD"))) {
		stdoutput.printf("mysql_thread_id\n");
		checkSuccess((mysql_thread_id(&mysql)!=0),1);
		stdoutput.printf("\n");
	}


	// some versions of mariadb crash when you call this
	// (arguably, I should figure out which versions and look for that too)
	#ifndef MARIADB_BASE_VERSION
	stdoutput.printf("mysql_list_processes\n");
	result=mysql_list_processes(&mysql);
	checkSuccess(mysql_num_fields(result),9);
	field=mysql_fetch_field_direct(result,0);
	checkSuccess(field->name,"Id");
	field=mysql_fetch_field_direct(result,1);
	checkSuccess(field->name,"User");
	field=mysql_fetch_field_direct(result,2);
	checkSuccess(field->name,"Host");
	field=mysql_fetch_field_direct(result,3);
	checkSuccess(field->name,"db");
	field=mysql_fetch_field_direct(result,4);
	checkSuccess(field->name,"Command");
	field=mysql_fetch_field_direct(result,5);
	checkSuccess(field->name,"Time");
	field=mysql_fetch_field_direct(result,6);
	checkSuccess(field->name,"State");
	field=mysql_fetch_field_direct(result,7);
	checkSuccess(field->name,"Info");
	field=mysql_fetch_field_direct(result,8);
	checkSuccess(field->name,"Progress");
	row=mysql_fetch_row(result);
	stdoutput.printf("\n");
	mysql_free_result(result);
	stdoutput.printf("\n");
	#endif

	// FIXME: mysql_info for:
	// insert into ... select ...
	// insert into ... values (...),(...),(...)...
	// load data infile ...
	// alter table
	// update

	// FIXME: mysql_change_user

	#if defined(MARIADB_BASE_VERSION) || \
		(defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID<80000)
	stdoutput.printf("mysql_shutdown\n");
	// should fail for lack of permissions
	// deprecated in real mysql, and always returns 1 on error
	checkSuccess(mysql_shutdown(&mysql,SHUTDOWN_DEFAULT),1);
	stdoutput.printf("\n");
	#endif

	stdoutput.printf("mysql_refresh\n");
	// these should all fail for lack of permissions
	checkSuccess(mysql_refresh(&mysql,REFRESH_GRANT),1);
	checkSuccess(mysql_refresh(&mysql,REFRESH_LOG),1);
	checkSuccess(mysql_refresh(&mysql,REFRESH_TABLES),1);
	checkSuccess(mysql_refresh(&mysql,REFRESH_HOSTS),1);
	checkSuccess(mysql_refresh(&mysql,REFRESH_STATUS),1);
	checkSuccess(mysql_refresh(&mysql,REFRESH_SLAVE),1);
	checkSuccess(mysql_refresh(&mysql,REFRESH_MASTER),1);
	/*if (argc==2) {
		checkSuccess(mysql_refresh(&mysql,REFRESH_THREADS),1);
	} else {
		// no-op in this case
		checkSuccess(mysql_refresh(&mysql,REFRESH_THREADS),0);
	}*/
	stdoutput.printf("\n");

	stdoutput.printf("mysql_reload\n");
	// should fail for lack of permissions
	checkSuccess(mysql_reload(&mysql),1);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stat\n");
	const char	*stat=mysql_stat(&mysql);
	checkSuccess(charstring::contains(stat,"Uptime: "),1);
	checkSuccess(charstring::contains(stat,"Threads: "),1);
	checkSuccess(charstring::contains(stat,"Questions: "),1);
	checkSuccess(charstring::contains(stat,"Slow queries: "),1);
	checkSuccess(charstring::contains(stat,"Opens: "),1);
	checkSuccess(charstring::contains(stat,"Flush tables: "),1);
	checkSuccess(charstring::contains(stat,"Open tables: "),1);
	checkSuccess(charstring::contains(stat,"Queries per second avg: "),1);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_kill\n");
	// should fail for lack of permissions (or invalid thread id)
	// deprecated in real mysql, and always returns 1 on error
	checkSuccess(mysql_kill(&mysql,0),1);
	stdoutput.printf("\n");

	// FIXME: mysql_options
	// (no options are currently supported, except against a real database)

	mysql_close(&mysql);


	stdoutput.printf("\n============ Statement API ============\n\n");

	// statement api...
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
	checkSuccess((int)(stmt!=NULL),1);
	stdoutput.printf("\n");
	stdoutput.printf("mysql_stmt_prepare: create\n");
	query="create table testdb.testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(2,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testtext text, testvarchar varchar(40), testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testtimestamp timestamp)";
	checkSuccess(mysql_stmt_prepare(stmt,query,
				charstring::length(query)),0);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_execute: create\n");
	checkSuccess(mysql_stmt_execute(stmt),0);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_prepare/execute: insert\n");
	query="insert into testdb.testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','text1','varchar1','tinytext1','mediumtext1','longtext1',NULL)";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_prepare/execute: insert\n");
	query="insert into testdb.testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','text2','varchar2','tinytext2','mediumtext2','longtext2',NULL)";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_prepare: select\n");
	query="select * from testdb.testtable";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_field_count:\n");
	checkSuccess(mysql_stmt_field_count(stmt),19);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_fetch_field/mysql_field_tell:\n");
	result=mysql_stmt_result_metadata(stmt);
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),1);
	checkSuccess(field->name,"testtinyint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),2);
	checkSuccess(field->name,"testsmallint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),3);
	checkSuccess(field->name,"testmediumint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),4);
	checkSuccess(field->name,"testint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),5);
	checkSuccess(field->name,"testbigint");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),6);
	checkSuccess(field->name,"testfloat");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),7);
	checkSuccess(field->name,"testreal");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),8);
	checkSuccess(field->name,"testdecimal");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),9);
	checkSuccess(field->name,"testdate");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),10);
	checkSuccess(field->name,"testtime");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),11);
	checkSuccess(field->name,"testdatetime");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),12);
	checkSuccess(field->name,"testyear");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),13);
	checkSuccess(field->name,"testchar");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),14);
	checkSuccess(field->name,"testtext");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),15);
	checkSuccess(field->name,"testvarchar");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),16);
	checkSuccess(field->name,"testtinytext");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),17);
	checkSuccess(field->name,"testmediumtext");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),18);
	checkSuccess(field->name,"testlongtext");
	field=mysql_fetch_field(result);
	checkSuccess(mysql_field_tell(result),19);
	checkSuccess(field->name,"testtimestamp");
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_bind_result:\n");
	MYSQL_BIND	fieldbind[19];
	char		fieldbuffer[19*1024];
	my_bool		fieldisnull[19];
	unsigned long	fieldlength[19];
	for (uint16_t i=0; i<19; i++) {
		bytestring::zero(&fieldbind[i],sizeof(MYSQL_BIND));
		fieldbind[i].buffer_type=MYSQL_TYPE_STRING;
		fieldbind[i].buffer=&fieldbuffer[i*1024];
		fieldbind[i].buffer_length=1024;
		fieldbind[i].is_null=&fieldisnull[i];
		fieldbind[i].length=&fieldlength[i];
	}
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_execute: select\n");
	checkSuccess(mysql_stmt_execute(stmt),0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_fetch:\n");
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess((const char *)fieldbind[0].buffer,"1");
	checkSuccess((const char *)fieldbind[1].buffer,"1");
	checkSuccess((const char *)fieldbind[2].buffer,"1");
	checkSuccess((const char *)fieldbind[3].buffer,"1");
	checkSuccess((const char *)fieldbind[4].buffer,"1");
	//checkSuccess((const char *)fieldbind[5].buffer,"1.1");
	//checkSuccess((const char *)fieldbind[6].buffer,"1.1");
	checkSuccess((const char *)fieldbind[7].buffer,"1.1");
	checkSuccess((const char *)fieldbind[8].buffer,"2001-01-01");
	checkSuccess((const char *)fieldbind[9].buffer,"01:00:00");
	checkSuccess((const char *)fieldbind[10].buffer,"2001-01-01 01:00:00");
	checkSuccess((const char *)fieldbind[11].buffer,"2001");
	checkSuccess((const char *)fieldbind[12].buffer,"char1");
	checkSuccess(!charstring::compare(
			(const char *)fieldbind[13].buffer,"text1",5),1);
	checkSuccess((const char *)fieldbind[14].buffer,"varchar1");
	checkSuccess(!charstring::compare(
			(const char *)fieldbind[15].buffer,"tinytext1",9),1);
	checkSuccess(!charstring::compare(
			(const char *)fieldbind[16].buffer,"mediumtext1",11),1);
	checkSuccess(!charstring::compare(
			(const char *)fieldbind[17].buffer,"longtext1",9),1);
	stdoutput.printf("\n");
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess((const char *)fieldbind[0].buffer,"2");
	checkSuccess((const char *)fieldbind[1].buffer,"2");
	checkSuccess((const char *)fieldbind[2].buffer,"2");
	checkSuccess((const char *)fieldbind[3].buffer,"2");
	checkSuccess((const char *)fieldbind[4].buffer,"2");
	//checkSuccess((const char *)fieldbind[5].buffer,"2.1");
	//checkSuccess((const char *)fieldbind[6].buffer,"2.1");
	checkSuccess((const char *)fieldbind[7].buffer,"2.1");
	checkSuccess((const char *)fieldbind[8].buffer,"2002-01-01");
	checkSuccess((const char *)fieldbind[9].buffer,"02:00:00");
	checkSuccess((const char *)fieldbind[10].buffer,"2002-01-01 02:00:00");
	checkSuccess((const char *)fieldbind[11].buffer,"2002");
	checkSuccess((const char *)fieldbind[12].buffer,"char2");
	checkSuccess(!charstring::compare(
			(const char *)fieldbind[13].buffer,"text2",5),1);
	checkSuccess((const char *)fieldbind[14].buffer,"varchar2");
	checkSuccess(!charstring::compare(
			(const char *)fieldbind[15].buffer,"tinytext2",9),1);
	checkSuccess(!charstring::compare(
			(const char *)fieldbind[16].buffer,"mediumtext2",11),1);
	checkSuccess(!charstring::compare(
			(const char *)fieldbind[17].buffer,"longtext2",9),1);
	stdoutput.printf("\n");
	checkSuccess(mysql_stmt_fetch(stmt),MYSQL_NO_DATA);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_prepare/execute: drop\n");
	query="drop table testdb.testtable";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_prepare/execute: select with odd NULLS\n");
	query="select NULL,1,NULL,1,NULL,1";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess(fieldisnull[0],1);
	checkSuccess(fieldisnull[1],0);
	checkSuccess(fieldisnull[2],1);
	checkSuccess(fieldisnull[3],0);
	checkSuccess(fieldisnull[4],1);
	checkSuccess(fieldisnull[5],0);
	#ifdef MARIADB_BASE_VERSION
	checkSuccess(mysql_stmt_fetch(stmt),100);
	#endif
	stdoutput.printf("\n");	

	stdoutput.printf("mysql_stmt_prepare/execute: select with even NULLS\n");
	query="select 1,NULL,1,NULL,1,NULL";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess(fieldisnull[0],0);
	checkSuccess(fieldisnull[1],1);
	checkSuccess(fieldisnull[2],0);
	checkSuccess(fieldisnull[3],1);
	checkSuccess(fieldisnull[4],0);
	checkSuccess(fieldisnull[5],1);
	#ifdef MARIADB_BASE_VERSION
	checkSuccess(mysql_stmt_fetch(stmt),100);
	#endif
	stdoutput.printf("\n");



	stdoutput.printf("mysql_stmt_prepare/execute: select with binds\n");
	query="select ?,?,?,?,?,?,?,?,?,?,?,?,?,?";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	MYSQL_BIND	bind[14];
	unsigned long	bindlength[14];
	my_bool		bindisnull[14];
	bytestring::zero(&bind,sizeof(bind));
	bytestring::zero(&bindlength,sizeof(bindlength));
	bytestring::zero(&bindisnull,sizeof(bindisnull));

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

	bind[8].buffer_type=MYSQL_TYPE_TINY_BLOB;
	bind[8].buffer=(void *)"tinyblob1";
	bind[8].buffer_length=9;
	bindlength[8]=9;
	bind[8].length=&bindlength[8];
	bindisnull[8]=0;
	bind[8].is_null=&bindisnull[8];

	bind[9].buffer_type=MYSQL_TYPE_MEDIUM_BLOB;
	bind[9].buffer=(void *)"mediumblob1";
	bind[9].buffer_length=11;
	bindlength[9]=11;
	bind[9].length=&bindlength[9];
	bindisnull[9]=0;
	bind[9].is_null=&bindisnull[9];

	bind[10].buffer_type=MYSQL_TYPE_LONG_BLOB;
	bind[10].buffer=(void *)"longblob1";
	bind[10].buffer_length=9;
	bindlength[10]=9;
	bind[10].length=&bindlength[10];
	bindisnull[10]=0;
	bind[10].is_null=&bindisnull[10];

	bind[11].buffer_type=MYSQL_TYPE_DATE;
	MYSQL_TIME	datetm;
	bytestring::zero(&datetm,sizeof(datetm));
	datetm.year=2001;
	datetm.month=1;
	datetm.day=2;
	bind[11].buffer=(void *)&datetm;
	bind[11].buffer_length=sizeof(datetm);
	bindlength[11]=sizeof(datetm);
	bind[11].length=&bindlength[11];
	bindisnull[11]=0;
	bind[11].is_null=&bindisnull[11];

	bind[12].buffer_type=MYSQL_TYPE_TIME;
	MYSQL_TIME	timetm;
	bytestring::zero(&timetm,sizeof(timetm));
	timetm.neg=1;
	timetm.day=1;
	timetm.hour=12;
	timetm.minute=10;
	timetm.second=11;
	bind[12].buffer=(void *)&timetm;
	bind[12].buffer_length=sizeof(timetm);
	bindlength[12]=sizeof(timetm);
	bind[12].length=&bindlength[12];
	bindisnull[12]=0;
	bind[12].is_null=&bindisnull[12];

	bind[13].buffer_type=MYSQL_TYPE_DATETIME;
	MYSQL_TIME	datetimetm;
	bytestring::zero(&datetimetm,sizeof(datetimetm));
	datetimetm.year=2001;
	datetimetm.month=1;
	datetimetm.day=2;
	datetimetm.hour=12;
	datetimetm.minute=10;
	datetimetm.second=11;
	bind[13].buffer=(void *)&datetimetm;
	bind[13].buffer_length=sizeof(datetimetm);
	bindlength[13]=sizeof(datetimetm);
	bind[13].length=&bindlength[13];
	bindisnull[13]=0;
	bind[13].is_null=&bindisnull[13];

	checkSuccess(mysql_stmt_bind_param(stmt,bind),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess((const char *)fieldbind[0].buffer,"1");
	checkSuccess((const char *)fieldbind[1].buffer,"1");
	checkSuccess((const char *)fieldbind[2].buffer,"1");
	checkSuccess((const char *)fieldbind[3].buffer,"1");
	//checkSuccess((const char *)fieldbind[4].buffer,"1.1");
	//checkSuccess((const char *)fieldbind[5].buffer,"1.1");
	checkSuccess((const char *)fieldbind[6].buffer,"string1");
	checkSuccess((const char *)fieldbind[7].buffer,"varstring1");
	checkSuccess((const char *)fieldbind[8].buffer,"tinyblob1");
	checkSuccess((const char *)fieldbind[9].buffer,"mediumblob1");
	checkSuccess((const char *)fieldbind[10].buffer,"longblob1");
	checkSuccess((const char *)fieldbind[11].buffer,"2001-01-02");
	checkSuccess((const char *)fieldbind[12].buffer,"-36:10:11");
	checkSuccess((const char *)fieldbind[13].buffer,"2001-01-02 12:10:11");
	#ifdef MARIADB_BASE_VERSION
	checkSuccess(mysql_stmt_fetch(stmt),100);
	#endif
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_prepare/execute: select with even null binds\n");
	query="select ?,?,?,?";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	bytestring::zero(&bind,sizeof(bind));
	bytestring::zero(&bindlength,sizeof(bindlength));
	bytestring::zero(&bindisnull,sizeof(bindisnull));

	bind[0].buffer_type=MYSQL_TYPE_TINY;
	bind[0].buffer=&tinyval;
	bind[0].buffer_length=sizeof(tinyval);
	bindlength[0]=sizeof(tinyval);
	bind[0].length=&bindlength[0];
	bindisnull[0]=0;
	bind[0].is_null=&bindisnull[0];

	bind[1].buffer_type=MYSQL_TYPE_TINY;
	bind[1].buffer=0;
	bind[1].buffer_length=0;
	bindlength[1]=0;
	bind[1].length=&bindlength[1];
	bindisnull[1]=1;
	bind[1].is_null=&bindisnull[1];

	bind[2].buffer_type=MYSQL_TYPE_TINY;
	bind[2].buffer=&tinyval;
	bind[2].buffer_length=sizeof(tinyval);
	bindlength[2]=sizeof(tinyval);
	bind[2].length=&bindlength[2];
	bindisnull[2]=0;
	bind[2].is_null=&bindisnull[2];

	bind[3].buffer_type=MYSQL_TYPE_TINY;
	bind[3].buffer=0;
	bind[3].buffer_length=0;
	bindlength[3]=0;
	bind[3].length=&bindlength[3];
	bindisnull[3]=1;
	bind[3].is_null=&bindisnull[3];

	checkSuccess(mysql_stmt_bind_param(stmt,bind),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess(bindisnull[0],0);
	checkSuccess(bindisnull[1],1);
	checkSuccess(bindisnull[2],0);
	checkSuccess(bindisnull[3],1);
	#ifdef MARIADB_BASE_VERSION
	checkSuccess(mysql_stmt_fetch(stmt),100);
	#endif
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_prepare/execute: select with odd null binds\n");
	query="select ?,?,?,?";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	bytestring::zero(&bind,sizeof(bind));
	bytestring::zero(&bindlength,sizeof(bindlength));
	bytestring::zero(&bindisnull,sizeof(bindisnull));

	bind[0].buffer_type=MYSQL_TYPE_TINY;
	bind[0].buffer=0;
	bind[0].buffer_length=0;
	bindlength[0]=0;
	bind[0].length=&bindlength[0];
	bindisnull[0]=1;
	bind[0].is_null=&bindisnull[0];

	bind[1].buffer_type=MYSQL_TYPE_TINY;
	bind[1].buffer=&tinyval;
	bind[1].buffer_length=sizeof(tinyval);
	bindlength[1]=sizeof(tinyval);
	bind[1].length=&bindlength[1];
	bindisnull[1]=0;
	bind[1].is_null=&bindisnull[1];

	bind[2].buffer_type=MYSQL_TYPE_TINY;
	bind[2].buffer=0;
	bind[2].buffer_length=0;
	bindlength[2]=0;
	bind[2].length=&bindlength[2];
	bindisnull[2]=1;
	bind[2].is_null=&bindisnull[2];

	bind[3].buffer_type=MYSQL_TYPE_TINY;
	bind[3].buffer=&tinyval;
	bind[3].buffer_length=sizeof(tinyval);
	bindlength[3]=sizeof(tinyval);
	bind[3].length=&bindlength[3];
	bindisnull[3]=0;
	bind[3].is_null=&bindisnull[3];

	checkSuccess(mysql_stmt_bind_param(stmt,bind),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess(fieldisnull[0],1);
	checkSuccess(fieldisnull[1],0);
	checkSuccess(fieldisnull[2],1);
	checkSuccess(fieldisnull[3],0);
	#ifdef MARIADB_BASE_VERSION
	checkSuccess(mysql_stmt_fetch(stmt),100);
	#endif
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_close:\n");
	checkSuccess(mysql_stmt_close(stmt),0);
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_prepare/execute: binary data\n");
	query="create table testdb.testtable (col1 longblob)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	const char	value[]={0,'"','"','\n'};
	stringbuffer	q;
	q.append("insert into testdb.testtable values (_binary'");
	q.append(value,sizeof(value));
	q.append("')");
	checkSuccess(mysql_real_query(&mysql,q.getString(),q.getSize()),0);
	stmt=mysql_stmt_init(&mysql);
	query="select col1 from testdb.testtable";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	for (uint16_t i=0; i<19; i++) {
		bytestring::zero(&fieldbind[i],sizeof(MYSQL_BIND));
		fieldbind[i].buffer_type=MYSQL_TYPE_STRING;
		fieldbind[i].buffer=&fieldbuffer[i*1024];
		fieldbind[i].buffer_length=1024;
		fieldbind[i].is_null=&fieldisnull[i];
		fieldbind[i].length=&fieldlength[i];
	}
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	stdoutput.printf("\n");
	checkSuccess(fieldlength[0],sizeof(value));
	checkSuccess(bytestring::compare(fieldbind[0].buffer,
					value,sizeof(value)),0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_close:\n");
	checkSuccess(mysql_stmt_close(stmt),0);
	stdoutput.printf("\n");


	stdoutput.printf("\n============ Info ============\n\n");

	stdoutput.printf("mysql_get_server_info: %s\n",
				mysql_get_server_info(&mysql));
	stdoutput.printf("mysql_get_client_info: %s\n",
				mysql_get_client_info());
	stdoutput.printf("mysql_get_host_info: %s\n",
				mysql_get_host_info(&mysql));
	stdoutput.printf("mysql_get_proto_info: %d\n",
				mysql_get_proto_info(&mysql));
	stdoutput.printf("\n");


	mysql_close(&mysql);
}
