// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <rudiments/environment.h>
#include <rudiments/stringbuffer.h>

#include "sapbench.h"

extern "C" {
	#include <ctpublic.h>
}

#define SAP_FETCH_AT_ONCE 10
#define SAP_MAX_SELECT_LIST_SIZE 256
#define SAP_MAX_ITEM_BUFFER_SIZE 2048


class sapbenchconnection : public benchconnection {
	friend class sapbenchcursor;
	public:
			sapbenchconnection(const char *connectstring,
						const char *dbtype);
			~sapbenchconnection();

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

class sapbenchcursor : public benchcursor {
	public:
			sapbenchcursor(benchconnection *con);
			~sapbenchcursor();

		bool	open();
		bool	query(const char *query, bool getcolumns);
		bool	close();

	private:
		sapbenchconnection	*sybbcon;

		CS_COMMAND	*cmd;
		CS_INT		resultstype;
		CS_INT		ncols;
		CS_DATAFMT	column[SAP_MAX_SELECT_LIST_SIZE];
		char		data[SAP_MAX_SELECT_LIST_SIZE]
						[SAP_FETCH_AT_ONCE]
						[SAP_MAX_ITEM_BUFFER_SIZE];
		CS_INT		datalength[SAP_MAX_SELECT_LIST_SIZE]
						[SAP_FETCH_AT_ONCE];
		CS_SMALLINT	nullindicator[SAP_MAX_SELECT_LIST_SIZE]
						[SAP_FETCH_AT_ONCE];
		CS_INT		rowcount;
};

sapbenchmarks::sapbenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug) :
					benchmarks(connectstring,db,
						queries,rows,cols,colsize,
						samples,rsbs,debug) {
	con=new sapbenchconnection(connectstring,db);
	cur=new sapbenchcursor(con);
}


sapbenchconnection::sapbenchconnection(
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

sapbenchconnection::~sapbenchconnection() {
}

bool sapbenchconnection::connect() {

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
		(CS_VOID *)sapbenchconnection::csMessageCallback,CS_UNUSED,
			(CS_INT *)NULL)
			!=CS_SUCCEED) {
		stdoutput.printf("cs_config (CS_MESSAGE_CB) failed\n");
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_CLIENTMSG_CB,
		(CS_VOID *)sapbenchconnection::clientMessageCallback)
			!=CS_SUCCEED) {
		stdoutput.printf("cs_config (CS_CLIENTMSG_CB) failed\n");
		return false;
	}
	if (ct_callback(context,NULL,CS_SET,CS_SERVERMSG_CB,
		(CS_VOID *)sapbenchconnection::serverMessageCallback)
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
		stdoutput.printf("connect: ct_results (CS_FAIL) failed\n");
		return false;
	}
	if (resultstype==CS_CMD_FAIL) {
		stdoutput.printf("connect: ct_results (CS_CMD_FAIL) failed\n");
		return false;
	}
	ct_cancel(NULL,cmd,CS_CANCEL_ALL);
	ct_cmd_drop(cmd);
	return true;
}

bool sapbenchconnection::disconnect() {

	// clean up
	ct_close(conn,CS_UNUSED);
	ct_con_drop(conn);
	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context);
	return true;
}

CS_RETCODE sapbenchconnection::csMessageCallback(CS_CONTEXT *ctxt, 
							CS_CLIENTMSG *msgp) {
	stdoutput.printf("message: %s\n",msgp->msgstring);
	return CS_SUCCEED;
}

CS_RETCODE sapbenchconnection::clientMessageCallback(CS_CONTEXT *ctxt, 
							CS_CONNECTION *cnn,
							CS_CLIENTMSG *msgp) {
	stdoutput.printf("message: %s\n",msgp->msgstring);
	return CS_SUCCEED;
}

CS_RETCODE sapbenchconnection::serverMessageCallback(CS_CONTEXT *ctxt, 
							CS_CONNECTION *cnn,
							CS_SERVERMSG *msgp) {
	stdoutput.printf("message: %s\n",msgp->text);
	return CS_SUCCEED;
}

sapbenchcursor::sapbenchcursor(benchconnection *con) : benchcursor(con) {
	sybbcon=(sapbenchconnection *)con;
}

sapbenchcursor::~sapbenchcursor() {
}

bool sapbenchcursor::open() {

	// allocate a command structure
	if (ct_cmd_alloc(sybbcon->conn,&cmd)!=CS_SUCCEED) {
		stdoutput.printf("ct_cmd_alloc failed\n");
		return false;
	}
	return true;
}

bool sapbenchcursor::query(const char *query, bool getcolumns) {

	// initialize number of columns
	ncols=0;

	bool	cursorcmd=false;
	if (!charstring::compare(query,"select ",7)) {

		// initiate a cursor command
		if (ct_cursor(cmd,CS_CURSOR_DECLARE,
					(CS_CHAR *)"1",
					(CS_INT)1,
					(CS_CHAR *)query,
					charstring::length(query),
					CS_READ_ONLY)!=CS_SUCCEED) {
			stdoutput.printf("ct_cursor (declare) failed\n");
			return false;
		}

		if (ct_cursor(cmd,CS_CURSOR_ROWS,
					NULL,CS_UNUSED,
					NULL,CS_UNUSED,
					(CS_INT)SAP_FETCH_AT_ONCE)!=
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

		cursorcmd=true;

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
			stdoutput.printf("query: ct_results (CS_FAIL) failed\n");
			return false;
		}
		if (resultstype==CS_CMD_FAIL) {
			stdoutput.printf("query: ct_results (CS_CMD_FAIL) failed\n");
			return false;
		}

		// DDL/DML
		if (!cursorcmd) {
			return (resultstype==CS_CMD_SUCCEED);
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
		column[i].maxlength=SAP_MAX_ITEM_BUFFER_SIZE;
		column[i].scale=CS_UNUSED;
		column[i].precision=CS_UNUSED;
		column[i].status=CS_UNUSED;
		column[i].count=SAP_FETCH_AT_ONCE;
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

bool sapbenchcursor::close() {

	// clean up
	ct_cmd_drop(cmd);
	return true;
}
