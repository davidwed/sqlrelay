extern "C" {
	#include <ctpublic.h>
}
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/bytestring.h>
#include <rudiments/environment.h>
#include <rudiments/stringbuffer.h>
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
		server="localhost";
		user="test";
		password="test";
		db="testdb";
	} else {
		// to run against a real instance, provide a server name
		// eg: ./tds mssql
		if (argc==2) {
			server=argv[1];
		} else {
			server="localhost";
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
				"'2001-01-01 13:01:01.000', "
				"1.23, "
				"1.23, "
				"'2001-01-01 13:01:01:000', "
				"1.23, "
				"1.23, "
				"1, "
				"'01020304-0102-0304-0102-030401020304', "
				"1.23, "
				"1.23, "
				"'2001-01-01', "
				"'13:01:01.000', "
				"'2001-01-01T13:01:01.000', "
				"'2001-01-01 13:01:01.000', "
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
				"'2002-02-02 14:02:02.000', "
				"2.34, "
				"2.34, "
				"'2002-02-02 14:02:02.000', "
				"2.34, "
				"2.34, "
				"2, "
				"'01020304-0102-0304-0102-030401020304', "
				"2.34, "
				"2.34, "
				"'2002-02-02', "
				"'14:02:02.000', "
				"'2002-02-02T14:02:02.000', "
				"'2002-02-02 14:02:02.000', "
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
	stdoutput.printf("\n");


	stdoutput.printf("ct_results:\n");
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

	checkSuccess(column[0].name,"testtinyint");
	checkSuccess(column[0].datatype,CS_TINYINT_TYPE);
	checkSuccess(column[0].format,CS_FMT_NULLTERM);
	checkSuccess(column[0].maxlength,1);
	checkSuccess(column[0].precision,0);
	checkSuccess(column[0].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[0].status,CS_UNUSED);
	checkSuccess(column[0].count,1);
	checkSuccess(column[0].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[1].name,"testbit");
	checkSuccess(column[1].datatype,CS_BIT_TYPE);
	checkSuccess(column[1].format,CS_FMT_NULLTERM);
	checkSuccess(column[1].maxlength,1);
	checkSuccess(column[1].precision,0);
	checkSuccess(column[1].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[1].status,CS_UNUSED);
	checkSuccess(column[1].count,1);
	checkSuccess(column[1].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[2].name,"testsmallint");
	checkSuccess(column[2].datatype,CS_SMALLINT_TYPE);
	checkSuccess(column[2].format,CS_FMT_NULLTERM);
	checkSuccess(column[2].maxlength,2);
	checkSuccess(column[2].precision,0);
	checkSuccess(column[2].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[2].status,CS_UNUSED);
	checkSuccess(column[2].count,1);
	checkSuccess(column[2].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[3].name,"testint");
	checkSuccess(column[3].datatype,CS_INT_TYPE);
	checkSuccess(column[3].format,CS_FMT_NULLTERM);
	checkSuccess(column[3].maxlength,4);
	checkSuccess(column[3].precision,0);
	checkSuccess(column[3].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[3].status,CS_UNUSED);
	checkSuccess(column[3].count,1);
	checkSuccess(column[3].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[4].name,"testsmalldatetime");
	checkSuccess(column[4].datatype,CS_DATETIME4_TYPE);
	checkSuccess(column[4].format,CS_FMT_NULLTERM);
	checkSuccess(column[4].maxlength,4);
	checkSuccess(column[4].precision,0);
	checkSuccess(column[4].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[4].status,CS_UNUSED);
	checkSuccess(column[4].count,1);
	checkSuccess(column[4].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[5].name,"testreal");
	checkSuccess(column[5].datatype,CS_REAL_TYPE);
	checkSuccess(column[5].format,CS_FMT_NULLTERM);
	checkSuccess(column[5].maxlength,4);
	checkSuccess(column[5].precision,0);
	checkSuccess(column[5].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[5].status,CS_UNUSED);
	checkSuccess(column[5].count,1);
	checkSuccess(column[5].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[6].name,"testmoney");
	checkSuccess(column[6].datatype,CS_MONEY_TYPE);
	checkSuccess(column[6].format,CS_FMT_NULLTERM);
	checkSuccess(column[6].maxlength,8);
	checkSuccess(column[6].precision,0);
	checkSuccess(column[6].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[6].status,CS_UNUSED);
	checkSuccess(column[6].count,1);
	checkSuccess(column[6].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[7].name,"testdatetime");
	checkSuccess(column[7].datatype,CS_DATETIME_TYPE);
	checkSuccess(column[7].format,CS_FMT_NULLTERM);
	checkSuccess(column[7].maxlength,8);
	checkSuccess(column[7].precision,0);
	checkSuccess(column[7].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[7].status,CS_UNUSED);
	checkSuccess(column[7].count,1);
	checkSuccess(column[7].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[8].name,"testfloat");
	checkSuccess(column[8].datatype,CS_FLOAT_TYPE);
	checkSuccess(column[8].format,CS_FMT_NULLTERM);
	checkSuccess(column[8].maxlength,8);
	checkSuccess(column[8].precision,0);
	checkSuccess(column[8].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[8].status,CS_UNUSED);
	checkSuccess(column[8].count,1);
	checkSuccess(column[8].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[9].name,"testsmallmoney");
	checkSuccess(column[9].datatype,CS_MONEY4_TYPE);
	checkSuccess(column[9].format,CS_FMT_NULLTERM);
	checkSuccess(column[9].maxlength,4);
	checkSuccess(column[9].precision,0);
	checkSuccess(column[9].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[9].status,CS_UNUSED);
	checkSuccess(column[9].count,1);
	checkSuccess(column[9].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[10].name,"testbigint");
	checkSuccess(column[10].datatype,CS_BIGINT_TYPE);
	checkSuccess(column[10].format,CS_FMT_NULLTERM);
	checkSuccess(column[10].maxlength,8);
	checkSuccess(column[10].precision,0);
	checkSuccess(column[10].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[10].status,CS_UNUSED);
	checkSuccess(column[10].count,1);
	checkSuccess(column[10].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[11].name,"testguid");
	checkSuccess(column[11].datatype,CS_UNIQUE_TYPE);
	checkSuccess(column[11].format,CS_FMT_NULLTERM);
	checkSuccess(column[11].maxlength,16);
	checkSuccess(column[11].precision,0);
	checkSuccess(column[11].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[11].status,CS_UNUSED);
	checkSuccess(column[11].count,1);
	checkSuccess(column[11].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[12].name,"testdecimal");
	checkSuccess(column[12].datatype,CS_DECIMAL_TYPE);
	checkSuccess(column[12].format,CS_FMT_NULLTERM);
	checkSuccess(column[12].maxlength,35);
	checkSuccess(column[12].precision,3);
	checkSuccess(column[12].scale,2);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[12].status,CS_UNUSED);
	checkSuccess(column[12].count,1);
	checkSuccess(column[12].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[13].name,"testnumeric");
	checkSuccess(column[13].datatype,CS_NUMERIC_TYPE);
	checkSuccess(column[13].format,CS_FMT_NULLTERM);
	checkSuccess(column[13].maxlength,35);
	checkSuccess(column[13].precision,3);
	checkSuccess(column[13].scale,2);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[13].status,CS_UNUSED);
	checkSuccess(column[13].count,1);
	checkSuccess(column[13].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[14].name,"testdate");
	//checkSuccess(column[14].datatype,CS_DATE_TYPE);
	checkSuccess(column[14].datatype,CS_CHAR_TYPE);		// #4652
	checkSuccess(column[14].format,CS_FMT_NULLTERM);
	// FIXME: 64 direct, 16 via relay
	//checkSuccess(column[14].maxlength,64);
	checkSuccess(column[14].precision,0);
	checkSuccess(column[14].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[14].status,CS_UNUSED);
	checkSuccess(column[14].count,1);
	checkSuccess(column[14].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[15].name,"testtime");
	//checkSuccess(column[15].datatype,CS_TIME_TYPE);
	checkSuccess(column[15].datatype,CS_CHAR_TYPE);		// #4652
	checkSuccess(column[15].format,CS_FMT_NULLTERM);
	// FIXME: 16/7/7 direct, 64/0/0 via relay
	//checkSuccess(column[15].maxlength,16);
	//checkSuccess(column[15].precision,7);
	//checkSuccess(column[15].scale,7);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[15].status,CS_UNUSED);
	checkSuccess(column[15].count,1);
	checkSuccess(column[15].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[16].name,"testdatetime2");
	//checkSuccess(column[16].datatype,CS_DATETIME_TYPE);
	checkSuccess(column[16].datatype,CS_CHAR_TYPE);		// #4652
	checkSuccess(column[16].format,CS_FMT_NULLTERM);
	// FIXME: 16/7/7 direct, 64/0/0 via relay
	//checkSuccess(column[16].maxlength,16);
	//checkSuccess(column[16].precision,7);
	//checkSuccess(column[16].scale,7);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[16].status,CS_UNUSED);
	checkSuccess(column[16].count,1);
	checkSuccess(column[16].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[17].name,"testdatetimeoffset");
	//checkSuccess(column[17].datatype,CS_DATETIMEOFFSET_TYPE);
	checkSuccess(column[17].datatype,CS_CHAR_TYPE);		// #4652
	checkSuccess(column[17].format,CS_FMT_NULLTERM);
	// FIXME: 16/7/7 direct, 64/0/0 via relay
	//checkSuccess(column[17].maxlength,16);
	//checkSuccess(column[17].precision,7);
	//checkSuccess(column[17].scale,7);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[17].status,CS_UNUSED);
	checkSuccess(column[17].count,1);
	checkSuccess(column[17].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[18].name,"testchar");
	checkSuccess(column[18].datatype,CS_CHAR_TYPE);
	checkSuccess(column[18].format,CS_FMT_NULLTERM);
	// FIXME: 40 direct, 160 via relay	#4783
	//checkSuccess(column[18].maxlength,40);
	checkSuccess(column[18].precision,0);
	checkSuccess(column[18].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[18].status,CS_UNUSED);
	checkSuccess(column[18].count,1);
	checkSuccess(column[18].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[19].name,"testvarchar");
	//checkSuccess(column[19].datatype,CS_VARCHAR_TYPE);
	checkSuccess(column[19].datatype,CS_CHAR_TYPE);		// #4652
	checkSuccess(column[19].format,CS_FMT_NULLTERM);
	// FIXME: 40 direct, 160 via relay	#4783
	//checkSuccess(column[19].maxlength,40);
	checkSuccess(column[19].precision,0);
	checkSuccess(column[19].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[19].status,CS_UNUSED);
	checkSuccess(column[19].count,1);
	checkSuccess(column[19].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[20].name,"testbinary");
	checkSuccess(column[20].datatype,CS_BINARY_TYPE);
	checkSuccess(column[20].format,CS_FMT_NULLTERM);
	checkSuccess(column[20].maxlength,40);
	checkSuccess(column[20].precision,0);
	checkSuccess(column[20].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[20].status,CS_UNUSED);
	checkSuccess(column[20].count,1);
	checkSuccess(column[20].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[21].name,"testvarbinary");
	//checkSuccess(column[21].datatype,CS_VARBINARY_TYPE);
	checkSuccess(column[21].datatype,CS_BINARY_TYPE);	// #4781
	checkSuccess(column[21].format,CS_FMT_NULLTERM);
	checkSuccess(column[21].maxlength,40);
	checkSuccess(column[21].precision,0);
	checkSuccess(column[21].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[21].status,CS_UNUSED);
	checkSuccess(column[21].count,1);
	checkSuccess(column[21].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[22].name,"testnvarchar");
	//checkSuccess(column[22].datatype,CS_NVARCHAR_TYPE);
	checkSuccess(column[22].datatype,CS_CHAR_TYPE);		// #4652
	checkSuccess(column[22].format,CS_FMT_NULLTERM);
	// FIXME: 40 direct, 160 via relay	#4783
	//checkSuccess(column[22].maxlength,40);
	checkSuccess(column[22].precision,0);
	checkSuccess(column[22].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[22].status,CS_UNUSED);
	checkSuccess(column[22].count,1);
	checkSuccess(column[22].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[23].name,"testnchar");
	//checkSuccess(column[23].datatype,CS_NCHAR_TYPE);
	checkSuccess(column[23].datatype,CS_CHAR_TYPE);		// #4652
	checkSuccess(column[23].format,CS_FMT_NULLTERM);
	// FIXME: 40 direct, 160 via relay	#4783
	//checkSuccess(column[23].maxlength,40);
	checkSuccess(column[23].precision,0);
	checkSuccess(column[23].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[23].status,CS_UNUSED);
	checkSuccess(column[23].count,1);
	checkSuccess(column[23].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[24].name,"testxml");
	//checkSuccess(column[24].datatype,CS_XML_TYPE);
	checkSuccess(column[24].datatype,CS_CHAR_TYPE);		// #4652
	checkSuccess(column[24].format,CS_FMT_NULLTERM);
	// maxlength limited by maxfieldlength via relay, but not directly
	//checkSuccess(column[24].maxlength,131068);
	checkSuccess(column[24].precision,0);
	checkSuccess(column[24].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[24].status,CS_UNUSED);
	checkSuccess(column[24].count,1);
	checkSuccess(column[24].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[25].name,"testtext");
	checkSuccess(column[25].datatype,CS_TEXT_TYPE);
	checkSuccess(column[25].format,CS_FMT_NULLTERM);
	// maxlength limited by maxfieldlength via relay, but not directly
	//checkSuccess(column[25].maxlength,131068);
	checkSuccess(column[25].precision,0);
	checkSuccess(column[25].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[25].status,CS_UNUSED);
	checkSuccess(column[25].count,1);
	checkSuccess(column[25].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[26].name,"testimage");
	checkSuccess(column[26].datatype,CS_IMAGE_TYPE);
	checkSuccess(column[26].format,CS_FMT_NULLTERM);
	// maxlength limited by maxfieldlength via relay, but not directly
	//checkSuccess(column[26].maxlength,131068);
	checkSuccess(column[26].precision,0);
	checkSuccess(column[26].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[26].status,CS_UNUSED);
	checkSuccess(column[26].count,1);
	checkSuccess(column[26].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");

	checkSuccess(column[27].name,"testntext");
	checkSuccess(column[27].datatype,CS_TEXT_TYPE);
	checkSuccess(column[27].format,CS_FMT_NULLTERM);
	// maxlength limited by maxfieldlength via relay, but not directly
	//checkSuccess(column[27].maxlength,131068);
	checkSuccess(column[27].precision,0);
	checkSuccess(column[27].scale,0);
	// FIXME: 48 direct, 0 via relay
	//checkSuccess(column[27].status,CS_UNUSED);
	checkSuccess(column[27].count,1);
	checkSuccess(column[27].usertype,CS_CHAR_TYPE);
	stdoutput.printf("\n");


	stdoutput.printf("ct_fetch:\n");
	CS_INT	rowsread;
	checkSuccess(ct_fetch(cmd,CS_UNUSED,CS_UNUSED,
					CS_UNUSED,&rowsread),CS_SUCCEED);
	checkSuccess(rowsread,1);
	stdoutput.printf("\n");


	stdoutput.printf("row data:\n");
	checkSuccess(data[0],"1");
	checkSuccess(*(datalength[0]),2);
	checkSuccess(*(nullindicator[0]),0);
	checkSuccess(data[1],"1");
	checkSuccess(*(datalength[1]),2);
	checkSuccess(*(nullindicator[1]),0);
	checkSuccess(data[2],"1");
	checkSuccess(*(datalength[2]),2);
	checkSuccess(*(nullindicator[2]),0);
	checkSuccess(data[3],"1");
	checkSuccess(*(datalength[3]),2);
	checkSuccess(*(nullindicator[3]),0);
	checkSuccess(data[4],"Jan  1 2001 01:01:00:000PM");
	checkSuccess(*(datalength[4]),27);
	checkSuccess(*(nullindicator[4]),0);
	// reals aren't converted to strings reliably enough to compare
	//checkSuccess(data[5],"1.23");
	checkSuccess(*(nullindicator[5]),0);
	checkSuccess(data[6],"1.23");
	checkSuccess(*(datalength[6]),5);
	checkSuccess(*(nullindicator[6]),0);
	checkSuccess(data[7],"Jan  1 2001 01:01:01:000PM");
	checkSuccess(*(datalength[7]),27);
	checkSuccess(*(nullindicator[7]),0);
	// floats aren't converted to strings reliably enough to compare
	//checkSuccess(data[8],"1.23");
	checkSuccess(*(nullindicator[8]),0);
	checkSuccess(data[9],"1.23");
	checkSuccess(*(datalength[9]),5);
	checkSuccess(*(nullindicator[9]),0);
	checkSuccess(data[10],"1");
	checkSuccess(*(datalength[10]),2);
	checkSuccess(*(nullindicator[10]),0);
	checkSuccess(data[11],"01020304-0102-0304-0102-030401020304");
	checkSuccess(*(datalength[11]),37);
	checkSuccess(*(nullindicator[11]),0);
	checkSuccess(data[12],"1.23");
	checkSuccess(*(datalength[12]),5);
	checkSuccess(*(nullindicator[12]),0);
	checkSuccess(data[13],"1.23");
	checkSuccess(*(datalength[13]),5);
	checkSuccess(*(nullindicator[13]),0);
	// FIXME: #4780 date, time, datetime2, datetimeoffset types
	//checkSuccess(data[14],"2001-01-01");
	//checkSuccess(data[15],"13:01:01.0000000");
	//checkSuccess(data[16],"2001-01-01 13:01:01.0000000");
	//checkSuccess(data[17],"2001-01-01 13:01:01.0000000 +00:00");
	checkSuccess(data[18],"char1                                   ");
	checkSuccess(*(datalength[18]),41);
	checkSuccess(*(nullindicator[18]),0);
	checkSuccess(data[19],"varchar1");
	checkSuccess(*(datalength[19]),9);
	checkSuccess(*(nullindicator[19]),0);
	checkSuccess(data[20],"62696e61727931000000000000000000000000000000000000000000000000000000000000000000");
	checkSuccess(*(datalength[20]),81);
	checkSuccess(*(nullindicator[20]),0);
	checkSuccess(data[21],"76617262696e61727931");
	checkSuccess(*(datalength[21]),21);
	checkSuccess(*(nullindicator[21]),0);
	checkSuccess(data[22],"nvarchar1");
	checkSuccess(*(datalength[22]),10);
	checkSuccess(*(nullindicator[22]),0);
	checkSuccess(data[23],"nchar1                                  ");
	checkSuccess(*(datalength[23]),41);
	checkSuccess(*(nullindicator[23]),0);
	checkSuccess(data[24],"xml1");
	checkSuccess(*(datalength[24]),5);
	checkSuccess(*(nullindicator[24]),0);
	checkSuccess(data[25],"text1");
	checkSuccess(*(datalength[25]),6);
	checkSuccess(*(nullindicator[25]),0);
	checkSuccess(data[26],"696d61676531");
	checkSuccess(*(datalength[26]),13);
	checkSuccess(*(nullindicator[26]),0);
	checkSuccess(data[27],"ntext1");
	checkSuccess(*(datalength[27]),7);
	checkSuccess(*(nullindicator[27]),0);
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
	checkSuccess(data[20],"62696e61727932000000000000000000000000000000000000000000000000000000000000000000");
	checkSuccess(data[21],"76617262696e61727932");
	checkSuccess(data[22],"nvarchar2");
	checkSuccess(data[23],"nchar2                                  ");
	checkSuccess(data[24],"xml2");
	checkSuccess(data[25],"text2");
	checkSuccess(data[26],"696d61676532");
	checkSuccess(data[27],"ntext2");
	stdoutput.printf("\n");


	stdoutput.printf("ct_results:\n");
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
