#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>

void checkSuccess(char *value, char *success) {
	//printf("\"%s\"=\"%s\"\n",value,success);

	if (!success) {
		if (!value) {
			printf("success ");
			return;
		} else {
			printf("failure ");
			exit(0);
		}
	}

	if (!strcmp(value,success)) {
		printf("success ");
	} else {
		printf("failure ");
		exit(0);
	}
}

void checkSuccess(int value, int success) {
	//printf("\"%d\"=\"%d\"\n",value,success);

	if (value==success) {
		printf("success ");
	} else {
		printf("failure ");
		exit(0);
	}
}

int	main(int argc, char **argv) {

	// usage...
	if (argc<5) {
		printf("usage: mysql host port socket user password\n");
		exit(0);
	}

	MYSQL	mysql;
#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	printf("mysql_init\n");
	checkSuccess((int)mysql_init(&mysql),(int)&mysql);
	printf("\n");
#endif

	char	*host=argv[1];
	char	*port=argv[2];
	char	*socket=argv[3];
	char	*user=argv[4];
	char	*password=argv[5];

#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	printf("mysql_real_connect\n");
#if MYSQL_VERSION_ID>=32200
	checkSuccess((int)mysql_real_connect(&mysql,host,user,password,"",
					atoi(port),socket,0),(int)&mysql);
#else
	checkSuccess((int)mysql_real_connect(&mysql,host,user,password,
					atoi(port),socket,0),(int)&mysql);
	// mysql_select_db...
#endif
#else
	checkSuccess((int)mysql_connect(&mysql,host,user,password),
					(int)mysql);
#endif
	printf("\n");

#ifdef HAVE_MYSQL_PING
	printf("mysql_ping\n");
	checkSuccess(mysql_ping(&mysql),0);
	printf("\n");
#endif

	printf("mysql_character_set_name:\n");
	checkSuccess((char *)mysql_character_set_name(&mysql),"latin1");
	printf("\n");

	char	*query="drop table testdb.testtable";
	mysql_real_query(&mysql,query,strlen(query));

	printf("mysql_real_query: create\n");
	query="create table testdb.testtable (testtinyint tinyint, testsmallint smallint, testmediumint mediumint, testint int, testbigint bigint, testfloat float, testreal real, testdecimal decimal(1,1), testdate date, testtime time, testdatetime datetime, testyear year, testchar char(40), testtext text, testvarchar varchar(40), testtinytext tinytext, testmediumtext mediumtext, testlongtext longtext, testtimestamp timestamp)";
	checkSuccess(mysql_real_query(&mysql,query,strlen(query)),0);
	printf("\n");

	printf("mysql_real_query: insert\n");
	query="insert into testdb.testtable values (1,1,1,1,1,1.1,1.1,1.1,'2001-01-01','01:00:00','2001-01-01 01:00:00','2001','char1','text1','varchar1','tinytext1','mediumtext1','longtext1',NULL)";
	checkSuccess(mysql_real_query(&mysql,query,strlen(query)),0);
	checkSuccess(mysql_affected_rows(&mysql),1);
	query="insert into testdb.testtable values (2,2,2,2,2,2.1,2.1,2.1,'2002-01-01','02:00:00','2002-01-01 02:00:00','2002','char2','text2','varchar2','tinytext2','mediumtext2','longtext2',NULL)";
	checkSuccess(mysql_real_query(&mysql,query,strlen(query)),0);
	checkSuccess(mysql_affected_rows(&mysql),1);
	printf("\n");

	// mysql_insert_id...
	// mysql_info...




	printf("mysql_real_query: select\n");
	query="select * from testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,strlen(query)),0);
	printf("\n");

	printf("mysql_store_result:\n");
	MYSQL_RES	*result=mysql_store_result(&mysql);

	printf("mysql_field_count:\n");
	checkSuccess(mysql_field_count(&mysql),19);
	printf("\n");

	printf("mysql_num_fields:\n");
	checkSuccess(mysql_num_fields(result),19);
	printf("\n");

	printf("mysql_num_rows:\n");
	checkSuccess(mysql_num_rows(result),2);
	printf("\n");

	printf("mysql_field_seek:\n");
	checkSuccess(mysql_field_seek(result,0),0);
	printf("\n");

	printf("mysql_fetch_field:\n");
	MYSQL_FIELD	*field;

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testtinyint");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testsmallint");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testmediumint");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testint");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testbigint");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testfloat");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testreal");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testdecimal");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testdate");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testtime");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testdatetime");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testyear");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testchar");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testtext");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testvarchar");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testtinytext");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testmediumtext");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testlongtext");

	field=mysql_fetch_field(result);
	checkSuccess(field->name,"testtimestamp");
	printf("\n");

	printf("mysql_field_seek:\n");
	checkSuccess(mysql_field_seek(result,0),19);
	printf("\n");

	// FIXME:
	printf("mysql_fetch_fields:\n");
	printf("\n");

	// FIXME:
	printf("mysql_fetch_field_direct:\n");
	printf("\n");

	// FIXME:
	printf("mysql_field_tell:\n");
	printf("\n");

	printf("mysql_fetch_row:\n");
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
	printf("\n");

	printf("mysql_fetch_lengths:\n");
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
	printf("\n");

	printf("mysql_fetch_row:\n");
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
	printf("\n");

	printf("mysql_eof:\n");
	checkSuccess(mysql_eof(result),1);
	printf("\n");

	mysql_free_result(result);




	printf("mysql_real_query: select\n");
	query="select * from testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,strlen(query)),0);
	printf("\n");

	printf("mysql_use_result:\n");
	result=mysql_use_result(&mysql);
	printf("\n");

	printf("mysql_fetch_row:\n");
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
	checkSuccess((int)mysql_fetch_row(result),0);
	printf("\n");




	// FIXME:
	printf("mysql_row_tell:\n");
	printf("\n");

	// FIXME:
	printf("mysql_data_seek:\n");
	printf("\n");

	// FIXME:
	printf("mysql_row_seek:\n");
	printf("\n");



	printf("mysql_real_query: drop\n");
	query="drop table testdb.testtable";
	checkSuccess(mysql_real_query(&mysql,query,strlen(query)),0);
	printf("\n");

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

	// mysql_get_server_info
	// mysql_get_client_info
	// mysql_get_host_info
	// mysql_get_proto_info

	// mysql_list_dbs
	// mysql_list_tables
	// mysql_list_fields

	// mysql_list_processes

	// mysql_options

	// mysql_escape_string
	// mysql_real_escape_string
	// mysql_odbc_escape_string
	
	mysql_close(&mysql);
}
