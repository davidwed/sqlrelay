// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "../config.h"

#include <rudiments/charstring.h>
#include <rudiments/environment.h>
#include <rudiments/datetime.h>

#include "mysqlbench.h"

mysqlbenchmarks::mysqlbenchmarks(const char *connectstring,
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
	con=new mysqlbenchconnection(connectstring,db);
	cur=new mysqlbenchcursor(con);
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


mysqlbenchcursor::mysqlbenchcursor(benchconnection *con) : benchcursor(con) {
	mbcon=(mysqlbenchconnection *)con;
}

mysqlbenchcursor::~mysqlbenchcursor() {
}

bool mysqlbenchcursor::query(const char *query, bool getcolumns) {
/*datetime	start;
datetime	end;
start.getSystemDateAndTime();*/

	#ifdef HAVE_MYSQL_COMMIT
	if (mbcon->firstquery) {
		if (mysql_commit(&mbcon->mysql)) {
			stdoutput.printf("mysql_commit failed\n");
			return false;
		}
		mbcon->firstquery=false;
	}
	#endif

	// execute the query
	if (mysql_real_query(&mbcon->mysql,query,
					charstring::length(query))) {
		return false;
	}

	// get the result set
	MYSQL_RES	*mysqlresult=mysql_store_result(&mbcon->mysql);
	if (mysqlresult) {

		// get column info
		uint32_t	ncols=mysql_num_fields(mysqlresult);

		// get the row count
		mysql_num_rows(mysqlresult);

		// get the affected row count
		mysql_affected_rows(&mbcon->mysql);

		if (getcolumns) {
			mysql_field_seek(mysqlresult,0);
			for (uint32_t i=0; i<ncols; i++) {
				MYSQL_FIELD	*mysqlfields=
					mysql_fetch_field(mysqlresult);
				//stdoutput.printf("%s,",mysqlfields->name);
			}
			//stdoutput.printf("\n");
		}

		// run through the rows
		MYSQL_ROW	mysqlrow;
		unsigned long	*mysqlrowlengths;
		while ((mysqlrow=mysql_fetch_row(mysqlresult)) &&
			(mysqlrowlengths=mysql_fetch_lengths(mysqlresult))) {
			/*stdoutput.printf("row...\n");
			for (uint32_t i=0; i<ncols; i++) {
				stdoutput.printf("  \"%s\"\n",mysqlrow[i]);
			}*/
		}
/*end.getSystemDateAndTime();
uint32_t	sec=end.getEpoch()-start.getEpoch();
int32_t		usec=end.getMicroseconds()-start.getMicroseconds();
if (usec<0) {
	sec--;
	usec=usec+1000000;
}
stdoutput.printf("% 4d.%06d\n",sec,usec);*/

		// free the result set
		mysql_free_result(mysqlresult);
	}
	return true;
}
