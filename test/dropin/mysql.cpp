#include <mysql.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/bytestring.h>
#include <rudiments/stdio.h>
#include <config.h>

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			return;
		} else {
			stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
			stdoutput.printf("failure ");
			process::exit(0);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
		stdoutput.printf("failure ");
		process::exit(0);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
	} else {
		stdoutput.printf("\"%d\"!=\"%d\"\n",value,success);
		stdoutput.printf("failure ");
		process::exit(0);
	}
}

int	main(int argc, char **argv) {

	MYSQL	mysql;
#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	stdoutput.printf("mysql_init\n");
	checkSuccess((long)mysql_init(&mysql),(long)&mysql);
	stdoutput.printf("\n");
#endif

	const char	*host="sqlrserver";
	const char	*port="9000";
	const char	*socket="/tmp/test.socket";
	const char	*user="test";
	const char	*password="test";

#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	stdoutput.printf("mysql_real_connect\n");
#if MYSQL_VERSION_ID>=32200
	checkSuccess((long)mysql_real_connect(&mysql,host,user,password,"",
					charstring::toInteger(port),
					socket,0),(long)&mysql);
#else
	checkSuccess((long)mysql_real_connect(&mysql,host,user,password,
					charstring::toInteger(port),
					socket,0),(long)&mysql);
	// mysql_select_db...
#endif
#else
	checkSuccess((long)mysql_connect(&mysql,host,user,password),
					(long)mysql);
#endif
	stdoutput.printf("\n");
#ifdef HAVE_MYSQL_PING
	stdoutput.printf("mysql_ping\n");
	checkSuccess(mysql_ping(&mysql),0);
	stdoutput.printf("\n");
#endif

	stdoutput.printf("mysql_character_set_name:\n");
	checkSuccess((char *)mysql_character_set_name(&mysql),"latin1");
	stdoutput.printf("\n");

	const char	*query="drop table testdb.testtable";
	mysql_real_query(&mysql,query,charstring::length(query));

	stdoutput.printf("mysql_real_query: create\n");
	query="create table testdb.testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(2,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testtext text, testvarchar varchar(40), testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testtimestamp timestamp)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_real_query: insert\n");
	query="insert into testdb.testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','text1','varchar1','tinytext1','mediumtext1','longtext1',NULL)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_affected_rows(&mysql),1);
	query="insert into testdb.testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','text2','varchar2','tinytext2','mediumtext2','longtext2',NULL)";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	checkSuccess(mysql_affected_rows(&mysql),1);
	stdoutput.printf("\n");

	// mysql_insert_id...
	// mysql_info...



	stdoutput.printf("mysql_real_query: select\n");
	query="select * from testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_field_count:\n");
	checkSuccess(mysql_field_count(&mysql),19);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_store_result:\n");
	MYSQL_RES	*result=mysql_store_result(&mysql);

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
	MYSQL_FIELD	*field;
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

	stdoutput.printf("mysql_field_seek:\n");
	checkSuccess(mysql_field_seek(result,0),19);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_fetch_field_direct:\n");
	field=mysql_fetch_field_direct(result,0);
	checkSuccess(field->name,"testtinyint");
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

#if 0
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
#endif

	stdoutput.printf("mysql_fetch_row:\n");
	MYSQL_ROW	row;
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"1");
	checkSuccess(row[1],"1");
	checkSuccess(row[2],"1");
	checkSuccess(row[3],"1");
	checkSuccess(row[4],"1");
	checkSuccess(row[5],"1.1");
	checkSuccess(row[6],"1.1");
	checkSuccess(row[7],"1.1");
	checkSuccess(row[8],"2001-01-01");
	checkSuccess(row[9],"01:00:00");
	checkSuccess(row[10],"2001-01-01 01:00:00");
	checkSuccess(row[11],"2001");
	checkSuccess(row[12],"char1");
	checkSuccess(row[13],"text1");
	checkSuccess(row[14],"varchar1");
	checkSuccess(row[15],"tinytext1");
	checkSuccess(row[16],"mediumtext1");
	checkSuccess(row[17],"longtext1");
	stdoutput.printf("\n");

	stdoutput.printf("mysql_fetch_lengths:\n");
	unsigned long	*lengths;
	lengths=mysql_fetch_lengths(result);
	checkSuccess(lengths[0],1);
	checkSuccess(lengths[1],1);
	checkSuccess(lengths[2],1);
	checkSuccess(lengths[3],1);
	checkSuccess(lengths[4],1);
	checkSuccess(lengths[5],3);
	checkSuccess(lengths[6],3);
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
	checkSuccess(row[5],"2.1");
	checkSuccess(row[6],"2.1");
	checkSuccess(row[7],"2.1");
	checkSuccess(row[8],"2002-01-01");
	checkSuccess(row[9],"02:00:00");
	checkSuccess(row[10],"2002-01-01 02:00:00");
	checkSuccess(row[11],"2002");
	checkSuccess(row[12],"char2");
	checkSuccess(row[13],"text2");
	checkSuccess(row[14],"varchar2");
	checkSuccess(row[15],"tinytext2");
	checkSuccess(row[16],"mediumtext2");
	checkSuccess(row[17],"longtext2");
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
	checkSuccess(row[5],"1.1");
	checkSuccess(row[6],"1.1");
	checkSuccess(row[7],"1.1");
	checkSuccess(row[8],"2001-01-01");
	checkSuccess(row[9],"01:00:00");
	checkSuccess(row[10],"2001-01-01 01:00:00");
	checkSuccess(row[11],"2001");
	checkSuccess(row[12],"char1");
	checkSuccess(row[13],"text1");
	checkSuccess(row[14],"varchar1");
	checkSuccess(row[15],"tinytext1");
	checkSuccess(row[16],"mediumtext1");
	checkSuccess(row[17],"longtext1");
	row=mysql_fetch_row(result);
	checkSuccess(row[0],"2");
	checkSuccess(row[1],"2");
	checkSuccess(row[2],"2");
	checkSuccess(row[3],"2");
	checkSuccess(row[4],"2");
	checkSuccess(row[5],"2.1");
	checkSuccess(row[6],"2.1");
	checkSuccess(row[7],"2.1");
	checkSuccess(row[8],"2002-01-01");
	checkSuccess(row[9],"02:00:00");
	checkSuccess(row[10],"2002-01-01 02:00:00");
	checkSuccess(row[11],"2002");
	checkSuccess(row[12],"char2");
	checkSuccess(row[13],"text2");
	checkSuccess(row[14],"varchar2");
	checkSuccess(row[15],"tinytext2");
	checkSuccess(row[16],"mediumtext2");
	checkSuccess(row[17],"longtext2");
	checkSuccess((long)mysql_fetch_row(result),0);
	stdoutput.printf("\n");

	mysql_free_result(result);


	stdoutput.printf("mysql_real_query: drop\n");
	query="drop table testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,charstring::length(query)),0);
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

	stdoutput.printf("mysql_get_*_info:\n");
	stdoutput.printf("server: %s\n",mysql_get_server_info(&mysql));
	stdoutput.printf("client: %s\n",mysql_get_client_info());
	stdoutput.printf("host: %s\n",mysql_get_host_info(&mysql));
	stdoutput.printf("proto: %d\n",mysql_get_proto_info(&mysql));
	stdoutput.printf("\n");
	

	// mysql_error
	// mysql_errno
	// mysql_thread_id
	// mysql_change_user

	// mysql_send_query
	// mysql_read_query_result

	// mysql_create_db
	// mysql_drop_db

	// mysql_shutdown
	// mysql_refresh
	// mysql_reload

	// mysql_debug
	// mysql_dump_debug_info

	// mysql_kill
	// mysql_stat

	// mysql_list_dbs
	// mysql_list_tables
	// mysql_list_fields

	// mysql_list_processes

	// mysql_options
		
	// mysql_odbc_escape_string
	// myodbc_remove_escape

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

	stdoutput.printf("mysql_stmt_prepare/execute: select\n");
	query="select * from testdb.testtable";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
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
	my_bool		isnull[19];
	unsigned long	fieldlength[19];
	for (uint16_t i=0; i<19; i++) {
		bytestring::zero(&fieldbind[i],sizeof(MYSQL_BIND));
		fieldbind[i].buffer_type=MYSQL_TYPE_STRING;
		fieldbind[i].buffer=&fieldbuffer[i*1024];
		fieldbind[i].buffer_length=1024;
		fieldbind[i].is_null=&isnull[i];
		fieldbind[i].length=&fieldlength[i];
	}
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_fetch:\n");
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess((const char *)fieldbind[0].buffer,"1");
	checkSuccess((const char *)fieldbind[1].buffer,"1");
	checkSuccess((const char *)fieldbind[2].buffer,"1");
	checkSuccess((const char *)fieldbind[3].buffer,"1");
	checkSuccess((const char *)fieldbind[4].buffer,"1");
	checkSuccess((const char *)fieldbind[5].buffer,"1.1");
	checkSuccess((const char *)fieldbind[6].buffer,"1.1");
	checkSuccess((const char *)fieldbind[7].buffer,"1.1");
	checkSuccess((const char *)fieldbind[8].buffer,"2001-01-01");
	checkSuccess((const char *)fieldbind[9].buffer,"01:00:00");
	checkSuccess((const char *)fieldbind[10].buffer,"2001-01-01 01:00:00");
	checkSuccess((const char *)fieldbind[11].buffer,"2001");
	checkSuccess((const char *)fieldbind[12].buffer,"char1");
	checkSuccess((const char *)fieldbind[13].buffer,"text1");
	checkSuccess((const char *)fieldbind[14].buffer,"varchar1");
	checkSuccess((const char *)fieldbind[15].buffer,"tinytext1");
	checkSuccess((const char *)fieldbind[16].buffer,"mediumtext1");
	checkSuccess((const char *)fieldbind[17].buffer,"longtext1");
	stdoutput.printf("\n");
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess((const char *)fieldbind[0].buffer,"2");
	checkSuccess((const char *)fieldbind[1].buffer,"2");
	checkSuccess((const char *)fieldbind[2].buffer,"2");
	checkSuccess((const char *)fieldbind[3].buffer,"2");
	checkSuccess((const char *)fieldbind[4].buffer,"2");
	checkSuccess((const char *)fieldbind[5].buffer,"2.1");
	checkSuccess((const char *)fieldbind[6].buffer,"2.1");
	checkSuccess((const char *)fieldbind[7].buffer,"2.1");
	checkSuccess((const char *)fieldbind[8].buffer,"2002-01-01");
	checkSuccess((const char *)fieldbind[9].buffer,"02:00:00");
	checkSuccess((const char *)fieldbind[10].buffer,"2002-01-01 02:00:00");
	checkSuccess((const char *)fieldbind[11].buffer,"2002");
	checkSuccess((const char *)fieldbind[12].buffer,"char2");
	checkSuccess((const char *)fieldbind[13].buffer,"text2");
	checkSuccess((const char *)fieldbind[14].buffer,"varchar2");
	checkSuccess((const char *)fieldbind[15].buffer,"tinytext2");
	checkSuccess((const char *)fieldbind[16].buffer,"mediumtext2");
	checkSuccess((const char *)fieldbind[17].buffer,"longtext2");
	stdoutput.printf("\n");
	checkSuccess(mysql_stmt_fetch(stmt),MYSQL_NO_DATA);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_prepare/execute: drop\n");
	query="drop table testdb.testtable";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_prepare/execute: select with even NULLS\n");
	query="select 1,NULL,1,NULL,1,NULL";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess(isnull[0],0);
	checkSuccess(isnull[1],1);
	checkSuccess(isnull[2],0);
	checkSuccess(isnull[3],1);
	checkSuccess(isnull[4],0);
	checkSuccess(isnull[5],1);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_prepare/execute: select with odd NULLS\n");
	query="select NULL,1,NULL,1,NULL,1";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	checkSuccess(mysql_stmt_execute(stmt),0);
	checkSuccess(mysql_stmt_bind_result(stmt,fieldbind),0);
	checkSuccess(mysql_stmt_fetch(stmt),0);
	checkSuccess(isnull[0],1);
	checkSuccess(isnull[1],0);
	checkSuccess(isnull[2],1);
	checkSuccess(isnull[3],0);
	checkSuccess(isnull[4],1);
	checkSuccess(isnull[5],0);
	stdoutput.printf("\n");

	stdoutput.printf("mysql_stmt_prepare/execute: select with binds\n");
	query="select ?,?,?,?,?,?,?,?,?,?,?";
	checkSuccess(mysql_stmt_prepare(stmt,query,charstring::length(query)),0);
	MYSQL_BIND	bind[19];
	unsigned long	bindlength[19];
	bytestring::zero(&bind,sizeof(bind));

	char	tinyval=1;
	bind[0].buffer_type=MYSQL_TYPE_TINY;
	bind[0].buffer=&tinyval;
	bind[0].buffer_length=sizeof(tinyval);
	bindlength[0]=sizeof(tinyval);
	bind[0].length=&bindlength[0];

	int16_t	shortval=1;
	bind[1].buffer_type=MYSQL_TYPE_SHORT;
	bind[1].buffer=&shortval;
	bind[1].buffer_length=sizeof(shortval);
	bindlength[1]=sizeof(shortval);
	bind[1].length=&bindlength[1];

	int32_t	longval=1;
	bind[2].buffer_type=MYSQL_TYPE_LONG;
	bind[2].buffer=&longval;
	bind[2].buffer_length=sizeof(longval);
	bindlength[2]=sizeof(longval);
	bind[2].length=&bindlength[2];

	int64_t	longlongval=1;
	bind[3].buffer_type=MYSQL_TYPE_LONGLONG;
	bind[3].buffer=&longlongval;
	bind[3].buffer_length=sizeof(longlongval);
	bindlength[3]=sizeof(longlongval);
	bind[3].length=&bindlength[3];

	float	floatval=1.1;
	bind[4].buffer_type=MYSQL_TYPE_FLOAT;
	bind[4].buffer=&floatval;
	bind[4].buffer_length=sizeof(floatval);
	bindlength[4]=sizeof(floatval);
	bind[4].length=&bindlength[4];

	double	doubleval=1.1;
	bind[5].buffer_type=MYSQL_TYPE_DOUBLE;
	bind[5].buffer=&doubleval;
	bind[5].buffer_length=sizeof(doubleval);
	bindlength[5]=sizeof(doubleval);
	bind[5].length=&bindlength[5];

	bind[6].buffer_type=MYSQL_TYPE_STRING;
	bind[6].buffer=(void *)"1";
	bind[6].buffer_length=1;
	bindlength[6]=1;
	bind[6].length=&bindlength[6];

	bind[7].buffer_type=MYSQL_TYPE_VAR_STRING;
	bind[7].buffer=(void *)"1";
	bind[7].buffer_length=1;
	bindlength[7]=1;
	bind[7].length=&bindlength[7];

	bind[8].buffer_type=MYSQL_TYPE_TINY_BLOB;
	bind[8].buffer=(void *)"1";
	bind[8].buffer_length=1;
	bindlength[8]=1;
	bind[8].length=&bindlength[8];

	bind[9].buffer_type=MYSQL_TYPE_MEDIUM_BLOB;
	bind[9].buffer=(void *)"1";
	bind[9].buffer_length=1;
	bindlength[9]=1;
	bind[9].length=&bindlength[9];

	bind[10].buffer_type=MYSQL_TYPE_LONG_BLOB;
	bind[10].buffer=(void *)"1";
	bind[10].buffer_length=1;
	bindlength[10]=1;
	bind[10].length=&bindlength[10];

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
	checkSuccess((const char *)fieldbind[6].buffer,"1");
	checkSuccess((const char *)fieldbind[7].buffer,"1");
	checkSuccess((const char *)fieldbind[8].buffer,"1");
	checkSuccess((const char *)fieldbind[9].buffer,"1");
	checkSuccess((const char *)fieldbind[10].buffer,"1");
	stdoutput.printf("\n");


	stdoutput.printf("mysql_stmt_close:\n");
	checkSuccess(mysql_stmt_close(stmt),0);
	stdoutput.printf("\n");


	mysql_close(&mysql);
}
