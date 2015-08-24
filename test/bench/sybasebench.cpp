// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <rudiments/environment.h>
#include <rudiments/stringbuffer.h>

#include "sybasebench.h"

extern "C" {
	#include <ctpublic.h>
}

#define SYBASE_FETCH_AT_ONCE 10
#define SYBASE_MAX_SELECT_LIST_SIZE 256
#define SYBASE_MAX_ITEM_BUFFER_SIZE 2048


class sybasebenchconnection : public benchconnection {
	friend class sybasebenchcursor;
	public:
			sybasebenchconnection(const char *connectstring,
						const char *dbtype);
			~sybasebenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*sybase;
		const char	*lang;
		const char	*server;
		const char	*dbname;
		const char	*user;
		const char	*password;

		CS_CONTEXT	*context;
		CS_LOCALE	*locale;
		CS_CONNECTION	*conn;

		static	CS_RETCODE	csMessageCallback(CS_CONTEXT *ctxt,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	clientMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_CLIENTMSG *msgp);
		static	CS_RETCODE	serverMessageCallback(CS_CONTEXT *ctxt,
						CS_CONNECTION *cnn,
						CS_SERVERMSG *msgp);
};

class sybasebenchcursor : public benchcursor {
	public:
			sybasebenchcursor(benchconnection *con);
			~sybasebenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		sybasebenchconnection	*sybbcon;

		CS_COMMAND	*cmd;
		CS_INT		resultstype;
		CS_INT		ncols;
		CS_DATAFMT	column[SYBASE_MAX_SELECT_LIST_SIZE];
		char		data[SYBASE_MAX_SELECT_LIST_SIZE]
						[SYBASE_FETCH_AT_ONCE]
						[SYBASE_MAX_ITEM_BUFFER_SIZE];
		CS_INT		datalength[SYBASE_MAX_SELECT_LIST_SIZE]
						[SYBASE_FETCH_AT_ONCE];
		CS_SMALLINT	nullindicator[SYBASE_MAX_SELECT_LIST_SIZE]
						[SYBASE_FETCH_AT_ONCE];
		CS_INT		rowcount;
};

sybasebenchmarks::sybasebenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug) :
					benchmarks(connectstring,db,
						queries,rows,cols,colsize,
						iterations,debug) {
	con=new sybasebenchconnection(connectstring,db);
	cur=new sybasebenchcursor(con);
}


sybasebenchconnection::sybasebenchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
	sybase=getParam("sybase");
	lang=getParam("lang");
	server=getParam("server");
	dbname=getParam("db");
	user=getParam("user");
	password=getParam("password");

	environment::setValue("SYBASE",sybase);
	environment::setValue("LANG",lang);
	environment::setValue("DSQUERY",server);
}

sybasebenchconnection::~sybasebenchconnection() {
}

bool sybasebenchconnection::connect() {

	// allocate a context
	context=(CS_CONTEXT *)NULL;
	if (cs_ctx_alloc(CS_VERSION_100,&context)!=CS_SUCCEED) {
		stdoutput.printf("cs_ctx_alloc failed\n");
		return false;
	}

	// init the context
	if (ct_init(context,CS_VERSION_100)!=CS_SUCCEED) {
		stdoutput.printf("ct_init failed\n");
		return false;
	}

	// configure the error handling callbacks
	if (cs_config(context,CS_SET,CS_MESSAGE_CB,
		(CS_VOID *)sybasebenchconnection::csMessageCallback,CS_UNUSED,
			(CS_INT *)NULL)
			!=CS_SUCCEED) {
		stdoutput.printf("cs_config (CS_MESSAGE_CB) failed\n");
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_CLIENTMSG_CB,
		(CS_VOID *)sybasebenchconnection::clientMessageCallback)
			!=CS_SUCCEED) {
		stdoutput.printf("cs_config (CS_CLIENTMSG_CB) failed\n");
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_SERVERMSG_CB,
		(CS_VOID *)sybasebenchconnection::serverMessageCallback)
			!=CS_SUCCEED) {
		stdoutput.printf("cs_config (CS_SERVERMSG_CB) failed\n");
		return false;
	}

	// allocate a connection
	if (ct_con_alloc(context,&conn)!=CS_SUCCEED) {
		stdoutput.printf("ct_con_alloc failed\n");
		return false;
	}

	// set the user/password to use
	if (ct_con_props(conn,CS_SET,CS_USERNAME,(CS_VOID *)user,
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		stdoutput.printf("ct_con_props (user) failed\n");
		return false;
	}

	if (ct_con_props(conn,CS_SET,CS_PASSWORD,(CS_VOID *)password,
				CS_NULLTERM,(CS_INT *)NULL)!=CS_SUCCEED) {
		stdoutput.printf("ct_con_props (password) failed\n");
		return false;
	}

	// connect to the database
	if (ct_connect(conn,(CS_CHAR *)NULL,(CS_INT)0)!=CS_SUCCEED) {
		stdoutput.printf("ct_connect failed...\n");
		return false;
	}

	// switch to the correct db
	CS_COMMAND	*cmd;
	if (ct_cmd_alloc(conn,&cmd)!=CS_SUCCEED) {
		stdoutput.printf("ct_cmd_alloc failed\n");
		return false;
	}
	stringbuffer	usedb;
	usedb.append("use ")->append(dbname);
	if (ct_command(cmd,CS_LANG_CMD,(CS_CHAR *)usedb.getString(),
			usedb.getStringLength(),CS_UNUSED)!=CS_SUCCEED) {
		stdoutput.printf("ct_command failed\n");
		return false;
	}
	if (ct_send(cmd)!=CS_SUCCEED) {
		stdoutput.printf("ct_send failed\n");
		return false;
	}
	CS_INT	resultstype;
	CS_INT	results=ct_results(cmd,&resultstype);
	if (results==CS_FAIL) {
		stdoutput.printf("ct_results (CS_FAIL) failed\n");
		return false;
	}
	if (resultstype==CS_CMD_FAIL) {
		stdoutput.printf("ct_results (CS_CMD_FAIL) failed\n");
		return false;
	}
	ct_cancel(NULL,cmd,CS_CANCEL_ALL);
	ct_cmd_drop(cmd);
	return true;
}

bool sybasebenchconnection::disconnect() {

	// clean up
	ct_close(conn,CS_UNUSED);
	ct_con_drop(conn);
	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context);
	return true;
}

CS_RETCODE sybasebenchconnection::csMessageCallback(CS_CONTEXT *ctxt, 
							CS_CLIENTMSG *msgp) {
	//stdoutput.printf("message: %s\n",msgp->msgstring);
	return CS_SUCCEED;
}

CS_RETCODE sybasebenchconnection::clientMessageCallback(CS_CONTEXT *ctxt, 
							CS_CONNECTION *cnn,
							CS_CLIENTMSG *msgp) {
	//stdoutput.printf("message: %s\n",msgp->msgstring);
	return CS_SUCCEED;
}

CS_RETCODE sybasebenchconnection::serverMessageCallback(CS_CONTEXT *ctxt, 
							CS_CONNECTION *cnn,
							CS_SERVERMSG *msgp) {
	//stdoutput.printf("message: %s\n",msgp->text);
	return CS_SUCCEED;
}

sybasebenchcursor::sybasebenchcursor(benchconnection *con) : benchcursor(con) {
	sybbcon=(sybasebenchconnection *)con;
}

sybasebenchcursor::~sybasebenchcursor() {
}

bool sybasebenchcursor::open() {

	// allocate a command structure
	if (ct_cmd_alloc(sybbcon->conn,&cmd)!=CS_SUCCEED) {
		stdoutput.printf("ct_cmd_alloc failed\n");
		return false;
	}
	return true;
}

bool sybasebenchcursor::query(const char *query, bool getcolumns) {

	// initialize number of columns
	ncols=0;

	bool	select=false;
	if (!charstring::compare(query,"select ",7)) {

		// initiate a cursor command
		if (ct_cursor(cmd,CS_CURSOR_DECLARE,
					(CS_CHAR *)"cur",
					(CS_INT)3,
					(CS_CHAR *)query,
					charstring::length(query),
					CS_READ_ONLY)!=CS_SUCCEED) {
			stdoutput.printf("ct_cursor (declare) failed\n");
			return false;
		}

		if (ct_cursor(cmd,CS_CURSOR_ROWS,
					NULL,CS_UNUSED,
					NULL,CS_UNUSED,
					(CS_INT)SYBASE_FETCH_AT_ONCE)!=
					CS_SUCCEED) {
			stdoutput.printf("ct_cursor (rows) failed\n");
			return false;
		}

		if (ct_cursor(cmd,CS_CURSOR_OPEN,
					NULL,CS_UNUSED,
					NULL,CS_UNUSED,
					CS_UNUSED)!=CS_SUCCEED) {
			stdoutput.printf("ct_cursor (open) failed\n");
			return false;
		}

		select=true;

	} else {

		// initiate a language command
		if (ct_command(cmd,CS_LANG_CMD,(CS_CHAR *)query,
			charstring::length(query),CS_UNUSED)!=CS_SUCCEED) {
			stdoutput.printf("ct_command failed\n");
			return false;
		}
	}

	// send the command
	if (ct_send(cmd)!=CS_SUCCEED) {
		stdoutput.printf("ct_send failed\n");
		return false;
	}

	// get the results
	for (;;) {

		CS_INT	results=ct_results(cmd,&resultstype);
		if (results==CS_FAIL) {
			stdoutput.printf("ct_results (CS_FAIL) failed\n");
			return false;
		}
		if (resultstype==CS_CMD_FAIL) {
			//stdoutput.printf("ct_results (CS_CMD_FAIL) failed\n");
			return false;
		}

		// DDL/DML
		if (!select && resultstype==CS_CMD_SUCCEED) {
			return true;
		}

		// select
		if (resultstype==CS_ROW_RESULT ||
				resultstype==CS_CURSOR_RESULT ||
				resultstype==CS_COMPUTE_RESULT) {
			break;
		}

		// cancel any result that isn't row-related
		ct_cancel(NULL,cmd,CS_CANCEL_CURRENT);
	}

	// get the number of columns
	if (ct_res_info(cmd,CS_NUMDATA,(CS_VOID *)&ncols,
				CS_UNUSED,(CS_INT *)NULL)!=CS_SUCCEED) {
		stdoutput.printf("ct_res_info failed\n");
		return false;
	}

	// for each column...
	for (CS_INT i=0; i<ncols; i++) {
	
		column[i].datatype=CS_CHAR_TYPE;
		column[i].format=CS_FMT_NULLTERM;
		column[i].maxlength=SYBASE_MAX_ITEM_BUFFER_SIZE;
		column[i].scale=CS_UNUSED;
		column[i].precision=CS_UNUSED;
		column[i].status=CS_UNUSED;
		column[i].count=SYBASE_FETCH_AT_ONCE;
		column[i].usertype=CS_UNUSED;
		column[i].locale=NULL;

		// bind the column for the fetches
		if (ct_bind(cmd,i+1,&column[i],(CS_VOID *)data[i],
				datalength[i],nullindicator[i])!=CS_SUCCEED) {
			stdoutput.printf("ct_bind failed\n");
			return false;
		}
	
		// get the column description
		if (ct_describe(cmd,i+1,&column[i])!=CS_SUCCEED) {
			stdoutput.printf("ct_describe failed\n");
			return false;
		}
	}
	
	// go fetch all rows and columns
	for (;;) {
		if (ct_fetch(cmd,CS_UNUSED,CS_UNUSED,
					CS_UNUSED,&rowcount)!=CS_SUCCEED &&
					!rowcount) {
			break;
		}

		// print row
		for (CS_INT row=0; row<rowcount; row++) {
			for (CS_INT col=0; col<ncols; col++) {
				if (nullindicator[col][row]>-1 && 
						datalength[col][row]) {
					//stdoutput.printf("%s,",data[col][row]);
				} else {
					//stdoutput.printf("NULL,");
				}
			}
			//stdoutput.printf("\n");
		}
	}

	// cancel any extra result sets
	ct_cancel(NULL,cmd,CS_CANCEL_CURRENT);

	// deallocate the cursor
	ct_cursor(cmd,CS_CURSOR_CLOSE,NULL,CS_UNUSED,NULL,CS_UNUSED,CS_DEALLOC);
	ct_send(cmd);
	ct_results(cmd,&resultstype);
	ct_cancel(NULL,cmd,CS_CANCEL_CURRENT);
	return true;
}

bool sybasebenchcursor::close() {

	// clean up
	ct_cmd_drop(cmd);
	return true;
}
