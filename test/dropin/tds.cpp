extern "C" {
	#include <ctpublic.h>
}
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/bytestring.h>
#include <rudiments/environment.h>
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
		process::exit(1);
	}
}

CS_RETCODE csMessageCallback(CS_CONTEXT *ctxt, CS_CLIENTMSG *msgp) {

	stringbuffer	errorstring;
	errorstring.append("Client Library error: ")->append(msgp->msgstring);
	errorstring.append(" severity(")->
		append((int32_t)CS_SEVERITY(msgp->msgnumber))->append(")");
	errorstring.append(" layer(")->
		append((int32_t)CS_LAYER(msgp->msgnumber))->append(")");
	errorstring.append(" origin(")->
		append((int32_t)CS_ORIGIN(msgp->msgnumber))->append(")");
	errorstring.append(" number(")->
		append((int32_t)CS_NUMBER(msgp->msgnumber))->append(")");

	if (msgp->osstringlen>0) {
		errorstring.append("  Operating System Error: ");
		errorstring.append(msgp->osstring);
	}

	stdoutput.printf("%s\n",errorstring.getString());

	return CS_SUCCEED;
}

CS_RETCODE clientMessageCallback(CS_CONTEXT *ctxt, 
					CS_CONNECTION *cnn,
					CS_CLIENTMSG *msgp) {
	stringbuffer	errorstring;
	errorstring.append("Client Library error: ")->append(msgp->msgstring);
	errorstring.append(" severity(")->
		append((int32_t)CS_SEVERITY(msgp->msgnumber))->append(")");
	errorstring.append(" layer(")->
		append((int32_t)CS_LAYER(msgp->msgnumber))->append(")");
	errorstring.append(" origin(")->
		append((int32_t)CS_ORIGIN(msgp->msgnumber))->append(")");
	errorstring.append(" number(")->
		append((int32_t)CS_NUMBER(msgp->msgnumber))->append(")");

	if (msgp->osstringlen>0) {
		errorstring.append("  Operating System Error: ");
		errorstring.append(msgp->osstring);
	}

	stdoutput.printf("%s\n",errorstring.getString());

	return CS_SUCCEED;
}

CS_RETCODE serverMessageCallback(CS_CONTEXT *ctxt, 
					CS_CONNECTION *cnn,
					CS_SERVERMSG *msgp) {
	stringbuffer	errorstring;
	errorstring.append("Server message: ")->append(msgp->text);
	errorstring.append(" severity(")->
		append((int32_t)CS_SEVERITY(msgp->msgnumber))->append(")");
	errorstring.append(" number(")->
		append((int32_t)CS_NUMBER(msgp->msgnumber))->append(")");
	errorstring.append(" state(")->
		append((int32_t)msgp->state)->append(")");
	errorstring.append(" line(")->
		append((int32_t)msgp->line)->append(")");
	errorstring.append("  Server Name:")->append(msgp->svrname);
	errorstring.append("  Procedure Name:")->append(msgp->proc);

	stdoutput.printf("%s\n",errorstring.getString());

	return CS_SUCCEED;
}

int	main(int argc, char **argv) {

	const char	*server;
	const char	*user;
	const char	*password;
	const char	*db;
	const char	*language="us_english";
	const char	*charset="utf-8";
	if (!charstring::isNullOrEmpty(environment::getValue("LD_PRELOAD"))) {
		server="sqlrelay";
		user="test";
		password="test";
		db="testdb";
	} else {
		// to run against a real instance, provide a server name
		// eg: ./tds mssql
		if (argc==2) {
			server=argv[1];
		} else {
			server="sqlrelay";
		}
		user="testuser";
		password="testpassword";
		db="testdb";
	}


	CS_CONTEXT	*context=NULL;
	CS_CONNECTION	*dbconn=NULL;

	environment::setValue("DSQUERY",server);

	stdoutput.printf("\n================ Login ================\n\n");

	stdoutput.printf("cs_ctx_alloc\n");
	checkSuccess(cs_ctx_alloc(CS_VERSION_100,&context),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("ct_init\n");
	checkSuccess(ct_init(context,CS_VERSION_100),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("ct_config: callbacks\n");
	checkSuccess(cs_config(context,CS_SET,CS_MESSAGE_CB,
				(CS_VOID *)csMessageCallback,
				CS_UNUSED,(CS_INT *)NULL),CS_SUCCEED);
	checkSuccess(ct_callback(context,NULL,CS_SET,CS_CLIENTMSG_CB,
				(CS_VOID *)clientMessageCallback),
				CS_SUCCEED);
	checkSuccess(ct_callback(context,NULL,CS_SET,CS_SERVERMSG_CB,
				(CS_VOID *)serverMessageCallback),
				CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_con_alloc\n");
	checkSuccess(ct_con_alloc(context,&dbconn),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_con_props: user\n");
	checkSuccess(ct_con_props(dbconn,CS_SET,
				CS_USERNAME,(CS_VOID *)user,CS_NULLTERM,
				(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_con_props: password\n");
	checkSuccess(ct_con_props(dbconn,CS_SET,
				CS_PASSWORD,(CS_VOID *)password,CS_NULLTERM,
				(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_con_props: appname\n");
	checkSuccess(ct_con_props(dbconn,CS_SET,
			CS_APPNAME,(CS_VOID *)"SQL Relay Test",CS_NULLTERM,
			(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_con_props: packet size\n");
	uint16_t	ps=4096;
	checkSuccess(ct_con_props(dbconn,CS_SET,
				CS_PACKETSIZE,(CS_VOID *)&ps,sizeof(ps),
				(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	#ifdef CS_SEC_ENCRYPTION
	stdoutput.printf("cs_con_props: sec encryption\n");
	CS_INT	enc=CS_TRUE;
	checkSuccess(ct_con_props(dbconn,CS_SET,
				CS_SEC_ENCRYPTION,(CS_VOID *)&enc,CS_UNUSED,
				(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	#endif
	stdoutput.printf("cs_loc_alloc\n");
	CS_LOCALE	*locale=NULL;
	checkSuccess(cs_loc_alloc(context,&locale),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_locale: lc_all\n");
	checkSuccess(cs_locale(context,CS_SET,
				locale,CS_LC_ALL,(CS_CHAR *)NULL,
				CS_UNUSED,(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_locale: language\n");
	checkSuccess(cs_locale(context,CS_SET,locale,
				CS_SYB_LANG,(CS_CHAR *)language,CS_NULLTERM,
				(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_locale: charset\n");
	checkSuccess(cs_locale(context,CS_SET,locale,
				CS_SYB_CHARSET,(CS_CHAR *)charset,CS_NULLTERM,
				(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("cs_con_props: locale\n");
	checkSuccess(ct_con_props(dbconn,CS_SET,
				CS_LOC_PROP,(CS_VOID *)locale,CS_UNUSED,
				(CS_INT *)NULL),CS_SUCCEED);
	stdoutput.printf("\n");
	stdoutput.printf("ct_connect\n");
	checkSuccess(ct_connect(dbconn,(CS_CHAR *)NULL,(CS_INT)0),CS_SUCCEED);
	stdoutput.printf("\n\n");



	stdoutput.printf("\n=============== Queries ===============\n\n");

	CS_INT		resultstype;

	stdoutput.printf("ct_cmd_alloc: cmd\n");
	CS_COMMAND	*cmd=NULL;
	checkSuccess(ct_cmd_alloc(dbconn,&cmd),CS_SUCCEED);
	stdoutput.printf("\n");

	stdoutput.printf("ct_command: use db\n");
	stringbuffer	q;
	q.append("use ")->append(db);
	const char	*query=q.getString();
	checkSuccess(ct_command(cmd,CS_LANG_CMD,
					query,charstring::length(query),
					CS_UNUSED),CS_SUCCEED);
	checkSuccess(ct_send(cmd),CS_SUCCEED);
	CS_INT	results=ct_results(cmd,&resultstype);
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_SUCCEED);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_DONE);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_END_RESULTS);
	checkSuccess(ct_cancel(NULL,cmd,CS_CANCEL_ALL),CS_SUCCEED);
	stdoutput.printf("\n");

	query="drop table testtable";
	ct_command(cmd,CS_LANG_CMD,query,charstring::length(query),CS_UNUSED);
	ct_send(cmd);
	while (ct_results(cmd,&resultstype)==CS_SUCCEED) {}
	ct_cancel(NULL,cmd,CS_CANCEL_ALL);

	stdoutput.printf("ct_command: create\n");
	query="create table testtable ("
				"testtinyint tinyint, "
				"testbit bit, "
				"testsmallint smallint, "
				"testint int, "
				"testsmalldatetime smalldatetime, "
				"testreal real, "
				"testmoney money, "
				"testdatetime datetime, "
				"testfloat float, "
				"testsmallmoney smallmoney, "
				"testbigint bigint, "
				"testguid uniqueidentifier, "
				"testdecimal decimal(3,2), "
				"testnumeric numeric(3,2), "
				"testdate date, "
				"testtime time, "
				"testdatetime2 datetime2, "
				"testdatetimeoffset datetimeoffset, "
				"testchar char(40), "
				"testvarchar varchar(40), "
				"testbinary binary(40), "
				"testvarbinary varbinary(40), "
				"testnvarchar nvarchar(40), "
				"testnchar nchar(40), "
				"testxml xml, "
				"testtext text, "
				"testimage image, "
				"testntext ntext"
				")";
	checkSuccess(ct_command(cmd,CS_LANG_CMD,
				query,charstring::length(query),
				CS_UNUSED),CS_SUCCEED);
	checkSuccess(ct_send(cmd),CS_SUCCEED);
	results=ct_results(cmd,&resultstype);
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_SUCCEED);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_DONE);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_END_RESULTS);
	checkSuccess(ct_cancel(NULL,cmd,CS_CANCEL_ALL),CS_SUCCEED);
	stdoutput.printf("\n");

	stdoutput.printf("ct_command: insert\n");
	query="insert into testtable values ("
				"1,"
				"1,"
				"1,"
				"1,"
				"'Jan 01 2001 01:01:01:000PM', "
				"1.23, "
				"1.23, "
				"'Jan 01 2001 01:01:01:000PM', "
				"1.23, "
				"1.23, "
				"1, "
				"'01020304-0102-0304-0102-030401020304', "
				"1.23, "
				"1.23, "
				"'Jan 01 2001', "
				"'01:01:01:000PM', "
				"'Jan 01 2001 01:01:01:000PM', "
				"'Jan 01 2001 01:01:01:000PM', "
				"'char1', "
				"'varchar1', "
				"CONVERT(binary, 'binary1'), "
				"CONVERT(varbinary, 'varbinary1'), "
				"'nvarchar1', "
				"'nchar1', "
				"'xml1', "
				"'text1', "
				"CONVERT(image, 'image1'), "
				"'ntext1'"
				")";
	checkSuccess(ct_command(cmd,CS_LANG_CMD,
					query,charstring::length(query),
					CS_UNUSED),CS_SUCCEED);
	checkSuccess(ct_send(cmd),CS_SUCCEED);
	results=ct_results(cmd,&resultstype);
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_SUCCEED);
	CS_INT	affectedrows;
	checkSuccess(ct_res_info(cmd,CS_ROW_COUNT,
					(CS_VOID *)&affectedrows,CS_UNUSED,
					(CS_INT *)NULL),CS_SUCCEED);
	checkSuccess(affectedrows,1);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_DONE);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_END_RESULTS);
	checkSuccess(ct_cancel(NULL,cmd,CS_CANCEL_ALL),CS_SUCCEED);
	stdoutput.printf("\n");

	stdoutput.printf("ct_command: insert\n");
	query="insert into testtable values ("
				"2,"
				"1,"
				"2,"
				"2,"
				"'Feb 02 2002 02:02:02:000PM', "
				"2.34, "
				"2.34, "
				"'Feb 02 2002 02:02:02:000PM', "
				"2.34, "
				"2.34, "
				"2, "
				"'01020304-0102-0304-0102-030401020304', "
				"2.34, "
				"2.34, "
				"'Feb 02 2002', "
				"'02:02:02:000PM', "
				"'Feb 02 2002 02:02:02:000PM', "
				"'Feb 02 2002 02:02:02:000PM', "
				"'char2', "
				"'varchar2', "
				"CONVERT(binary, 'binary2'), "
				"CONVERT(varbinary, 'varbinary2'), "
				"'nvarchar2', "
				"'nchar2', "
				"'xml2', "
				"'text2', "
				"CONVERT(image, 'image2'), "
				"'ntext2'"
				")";
	checkSuccess(ct_command(cmd,CS_LANG_CMD,
					query,charstring::length(query),
					CS_UNUSED),CS_SUCCEED);
	checkSuccess(ct_send(cmd),CS_SUCCEED);
	results=ct_results(cmd,&resultstype);
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_SUCCEED);
	checkSuccess(ct_res_info(cmd,CS_ROW_COUNT,
					(CS_VOID *)&affectedrows,CS_UNUSED,
					(CS_INT *)NULL),CS_SUCCEED);
	checkSuccess(affectedrows,1);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_DONE);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_END_RESULTS);
	checkSuccess(ct_cancel(NULL,cmd,CS_CANCEL_ALL),CS_SUCCEED);
	stdoutput.printf("\n");

	stdoutput.printf("ct_command: select\n");
	query="select * from testtable";
	checkSuccess(ct_command(cmd,CS_LANG_CMD,
					query,charstring::length(query),
					CS_UNUSED),CS_SUCCEED);
	checkSuccess(ct_send(cmd),CS_SUCCEED);
	results=ct_results(cmd,&resultstype);
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_ROW_RESULT);
	stdoutput.printf("\n");

	stdoutput.printf("ct_res_info: col count\n");
	CS_INT	ncols;
	checkSuccess(ct_res_info(cmd,CS_NUMDATA,
					(CS_VOID *)&ncols,CS_UNUSED,
					(CS_INT *)NULL),CS_SUCCEED);
	checkSuccess(ncols,28);
	stdoutput.printf("\n");

	stdoutput.printf("ct_bind:\n");
	CS_DATAFMT	column[28];
	char		*data[28];
	CS_INT		*datalength[28];
	CS_SMALLINT	*nullindicator[28];
	for (CS_INT i=0; i<ncols; i++) {
		data[i]=new char[1024];
		bytestring::zero(data[i],1024);
		datalength[i]=new CS_INT[1];
		nullindicator[i]=new CS_SMALLINT[1];

		column[i].datatype=CS_CHAR_TYPE;
		column[i].format=CS_FMT_NULLTERM;
		column[i].maxlength=1024;
		column[i].scale=CS_UNUSED;
		column[i].precision=CS_UNUSED;
		column[i].status=CS_UNUSED;
		column[i].count=1;
		column[i].usertype=CS_UNUSED;
		column[i].locale=NULL;
		checkSuccess(ct_bind(cmd,i+1,&(column[i]),
						(CS_VOID *)data[i],
						datalength[i],
						nullindicator[i]),
						CS_SUCCEED);
	}
	stdoutput.printf("\n");

	stdoutput.printf("ct_describe:\n");
	for (CS_INT i=0; i<ncols; i++) {
		checkSuccess(ct_describe(cmd,i+1,&(column[i])),CS_SUCCEED);
	}
	stdoutput.printf("\n");

#if 0
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
#endif

	stdoutput.printf("ct_fetch:\n");
	CS_INT	rowsread;
	checkSuccess(ct_fetch(cmd,CS_UNUSED,CS_UNUSED,
					CS_UNUSED,&rowsread),CS_SUCCEED);
	checkSuccess(rowsread,1);
	stdoutput.printf("\n");

	stdoutput.printf("row data:\n");
	checkSuccess(data[0],"1");
	checkSuccess(data[1],"1");
	checkSuccess(data[2],"1");
	checkSuccess(data[3],"1");
	checkSuccess(data[4],"Jan  1 2001 01:01:00:000PM");
	//checkSuccess(data[5],"1.23");
	checkSuccess(data[6],"1.23");
	checkSuccess(data[7],"Jan  1 2001 01:01:01:000PM");
	//checkSuccess(data[8],"1.23");
	checkSuccess(data[9],"1.23");
	checkSuccess(data[10],"1");
	checkSuccess(data[11],"01020304-0102-0304-0102-030401020304");
	checkSuccess(data[12],"1.23");
	checkSuccess(data[13],"1.23");
	//checkSuccess(data[14],"2001-01-01");		#4780
	//checkSuccess(data[15],"13:01:01.0000000");	#4780
	//checkSuccess(data[16],"2001-01-01 13:01:01.0000000");		#4780
	//checkSuccess(data[17],"2001-01-01 13:01:01.0000000 +00:00");	#4780
	checkSuccess(data[18],"char1                                   ");
	checkSuccess(data[19],"varchar1");
	//checkSuccess(data[20],"binary1");
	//checkSuccess(data[21],"varbinary1");
	checkSuccess(data[22],"nvarchar1");
	checkSuccess(data[23],"nchar1                                  ");
	checkSuccess(data[24],"xml1");
	checkSuccess(data[25],"text1");
	//checkSuccess(data[26],"image1");
	checkSuccess(data[27],"ntext1");
	stdoutput.printf("\n");

	stdoutput.printf("ct_fetch:\n");
	checkSuccess(ct_fetch(cmd,CS_UNUSED,CS_UNUSED,
					CS_UNUSED,&rowsread),CS_SUCCEED);
	checkSuccess(rowsread,1);
	stdoutput.printf("\n");

	stdoutput.printf("row data:\n");
	checkSuccess(data[0],"2");
	checkSuccess(data[1],"1");
	checkSuccess(data[2],"2");
	checkSuccess(data[3],"2");
	checkSuccess(data[4],"Feb  2 2002 02:02:00:000PM");
	//checkSuccess(data[5],"2.34");
	checkSuccess(data[6],"2.34");
	checkSuccess(data[7],"Feb  2 2002 02:02:02:000PM");
	//checkSuccess(data[8],"2.34");
	checkSuccess(data[9],"2.34");
	checkSuccess(data[10],"2");
	checkSuccess(data[11],"01020304-0102-0304-0102-030401020304");
	checkSuccess(data[12],"2.34");
	checkSuccess(data[13],"2.34");
	//checkSuccess(data[14],"2002-02-02");		#4780
	//checkSuccess(data[15],"14:02:02.0000000");	#4780
	//checkSuccess(data[16],"2002-02-02 14:02:02.0000000");		#4780
	//checkSuccess(data[17],"2002-02-02 14:02:02.0000000 +00:00");	#4780
	checkSuccess(data[18],"char2                                   ");
	checkSuccess(data[19],"varchar2");
	//checkSuccess(data[20],"binary2");
	//checkSuccess(data[21],"varbinary2");
	checkSuccess(data[22],"nvarchar2");
	checkSuccess(data[23],"nchar2                                  ");
	checkSuccess(data[24],"xml2");
	checkSuccess(data[25],"text2");
	//checkSuccess(data[26],"image2");
	checkSuccess(data[27],"ntext2");
	stdoutput.printf("\n");

#if 0
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
#endif

	stdoutput.printf("ct_command: ...select\n");
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_DONE);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_END_RESULTS);
	checkSuccess(ct_cancel(NULL,cmd,CS_CANCEL_ALL),CS_SUCCEED);
	stdoutput.printf("\n");


	stdoutput.printf("ct_command: drop\n");
	query="drop table testtable";
	checkSuccess(ct_command(cmd,CS_LANG_CMD,
					query,charstring::length(query),
					CS_UNUSED),CS_SUCCEED);
	checkSuccess(ct_send(cmd),CS_SUCCEED);
	results=ct_results(cmd,&resultstype);
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_SUCCEED);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_SUCCEED);
	checkSuccess(resultstype,CS_CMD_DONE);
	results=ct_results(cmd,&resultstype),
	checkSuccess(results,CS_END_RESULTS);
	checkSuccess(ct_cancel(NULL,cmd,CS_CANCEL_ALL),CS_SUCCEED);
	stdoutput.printf("\n");
}
