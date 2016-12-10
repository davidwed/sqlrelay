// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../../config.h"

#include <rudiments/charstring.h>
#include <rudiments/environment.h>
#include <rudiments/datetime.h>
#include <rudiments/process.h>

#include "mysqlbench.h"

#ifdef _WIN32
#include <winsock2.h>
#endif

extern "C" {
	#include <mysql.h>
}

class mysqlbenchconnection : public benchconnection {
	friend class mysqlbenchcursor;
	public:
			mysqlbenchconnection(const char *connectstring,
						const char *dbtype);
			~mysqlbenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*host;
		uint16_t	port;
		const char	*socket;
		const char	*dbname;
		const char	*user;
		const char	*password;

		const char	*sslcapath;
		const char	*sslca;
		const char	*sslcert;
		const char	*sslkey;
		const char	*sslcipher;

		MYSQL		mysql;

		bool		firstquery;
};

class mysqlbenchcursor : public benchcursor {
	public:
			mysqlbenchcursor(benchconnection *con,
						uint32_t cols,
						uint32_t colsize);
			~mysqlbenchcursor();

		bool	query(const char *query, bool getcolumns);

	private:
		mysqlbenchconnection	*mbcon;

		#ifdef HAVE_MYSQL_STMT_PREPARE
		uint32_t	colsize;
		MYSQL_STMT	*stmt;
		MYSQL_BIND	*fieldbind;
		char		*field;
		my_bool		*isnull;
		unsigned long	*fieldlength;
		#endif
};

mysqlbenchmarks::mysqlbenchmarks(const char *connectstring,
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
	con=new mysqlbenchconnection(connectstring,db);
	cur=new mysqlbenchcursor(con,cols,colsize);
}


mysqlbenchconnection::mysqlbenchconnection(
				const char *connectstring,
				const char *db) :
				benchconnection(connectstring,db) {
	host=getParam("host");
	port=charstring::toInteger(getParam("port"));
	socket=getParam("socket");
	dbname=getParam("db");
	user=getParam("user");
	password=getParam("password");
	sslcapath=getParam("sslcapath");
	sslca=getParam("sslca");
	sslcert=getParam("sslcert");
	sslkey=getParam("sslkey");
	sslcipher=getParam("sslcipher");
}

mysqlbenchconnection::~mysqlbenchconnection() {
}

bool mysqlbenchconnection::connect() {

	// init
	#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	#if MYSQL_VERSION_ID>=32200
	if (!mysql_init(&mysql)) {
		stdoutput.printf("mysql_init failed\n");
		return false;
	}

	#ifdef HAVE_MYSQL_SSL_SET
	mysql_ssl_set(&mysql,sslkey,sslcert,sslca,sslcapath,sslcipher);
	#endif
	
	// log in
	if (!mysql_real_connect(&mysql,host,user,password,
						dbname,port,socket,0)) {
	#else
	if (!mysql_real_connect(&mysql,host,user,password,port,socket,0)) {
	#endif
	#else
	if (!mysql_connect(&mysql,host,user,password)) {
	#endif
		stdoutput.printf("mysql_(real)_connect failed\n");
		return false;
	}
	#ifdef MYSQL_SELECT_DB
	if (mysql_select_db(&mysql,dbname)) {
		stdoutput.printf("mysql_select_db failed\n");
		mysql_close(&mysql);
		return false;
	}
	#endif

	firstquery=true;

	return true;
}

bool mysqlbenchconnection::disconnect() {
	mysql_close(&mysql);
	return true;
}

mysqlbenchcursor::mysqlbenchcursor(benchconnection *con,
						uint32_t cols,
						uint32_t colsize) :
						benchcursor(con) {
	mbcon=(mysqlbenchconnection *)con;
	#ifdef HAVE_MYSQL_STMT_PREPARE
	stmt=mysql_stmt_init(&mbcon->mysql);

	this->colsize=colsize;
	fieldbind=new MYSQL_BIND[cols];
	field=new char[cols*colsize];
	isnull=new my_bool[cols];
	fieldlength=new unsigned long[cols];
	for (unsigned short index=0; index<cols; index++) {
		bytestring::zero(&fieldbind[index],sizeof(MYSQL_BIND));
		fieldbind[index].buffer_type=MYSQL_TYPE_STRING;
		fieldbind[index].buffer=&field[index*colsize];
		fieldbind[index].buffer_length=colsize;
		fieldbind[index].is_null=&isnull[index];
		fieldbind[index].length=&fieldlength[index];
	}
	#endif
}

mysqlbenchcursor::~mysqlbenchcursor() {
	#ifdef HAVE_MYSQL_STMT_PREPARE
	delete[] fieldbind;
	delete[] field;
	delete[] isnull;
	delete[] fieldlength;
	#endif
}

bool mysqlbenchcursor::query(const char *query, bool getcolumns) {

	#ifdef HAVE_MYSQL_COMMIT
	if (mbcon->firstquery) {
		if (mysql_commit(&mbcon->mysql)) {
			stdoutput.printf("mysql_commit failed\n");
			return false;
		}
		mbcon->firstquery=false;
	}
	#endif

#ifdef HAVE_MYSQL_STMT_PREPARE
	if (charstring::compare(query,"create",6) && 
		charstring::compare(query,"drop",4) && 
		charstring::compare(query,"insert",6)) {

		// prepare the query
		if (mysql_stmt_prepare(stmt,query,charstring::length(query))) {
			return false;
		}

		// get the column count
		uint32_t	ncols=mysql_stmt_field_count(stmt);

		// get statement metadata
		MYSQL_RES	*mysqlresult=mysql_stmt_result_metadata(stmt);

		// bind result set buffers
		if (mysql_stmt_bind_result(stmt,fieldbind)) {
			return false;
		}

		// execute the query
		if (mysql_stmt_execute(stmt)) {
			return false;
		}
	
		// get the affected row count
		mysql_stmt_affected_rows(stmt);

		// run through the columns
		if (getcolumns) {
			mysql_field_seek(mysqlresult,0);
			for (uint32_t i=0; i<ncols; i++) {
				mysql_fetch_field(mysqlresult);
			}
		}

		// run through the rows
		while (!mysql_stmt_fetch(stmt)) {
		}

		// free the result set
		mysql_stmt_reset(stmt);
		mysql_stmt_free_result(stmt);

		return true;
	}
#endif
	// execute the query
	if (mysql_real_query(&mbcon->mysql,query,charstring::length(query))) {
		return false;
	}

	// get the result set
	MYSQL_RES	*mysqlresult=mysql_store_result(&mbcon->mysql);
	if (mysqlresult) {

		// get the column count
		uint32_t	ncols=mysql_num_fields(mysqlresult);

		// get the row count
		mysql_num_rows(mysqlresult);

		// get the affected row count
		mysql_affected_rows(&mbcon->mysql);

		// run through the columns
		if (getcolumns) {
			mysql_field_seek(mysqlresult,0);
			for (uint32_t i=0; i<ncols; i++) {
				mysql_fetch_field(mysqlresult);
			}
		}

		// run through the rows
		MYSQL_ROW	mysqlrow;
		unsigned long	*mysqlrowlengths;
		while ((mysqlrow=mysql_fetch_row(mysqlresult)) &&
			(mysqlrowlengths=mysql_fetch_lengths(mysqlresult))) {
		}

		// free the result set
		mysql_free_result(mysqlresult);
	}
	return true;
}
