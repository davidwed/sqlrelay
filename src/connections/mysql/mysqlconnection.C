// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <mysqlconnection.h>
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID>=32200
	#include <errmsg.h>
#endif

#include <datatypes.h>

#include <config.h>

#include <stdlib.h>

mysqlconnection::mysqlconnection() {
	connected=0;
}

int mysqlconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void mysqlconnection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	db=connectStringValue("db");
	host=connectStringValue("host");
	port=connectStringValue("port");
	socket=connectStringValue("socket");
}

bool mysqlconnection::logIn() {

	// Handle host.
	// For really old versions of mysql, a NULL host indicates that the
	// unix socket should be used.  There's no way to specify what unix
	// socket or inet port to connect to, those values are hardcoded
	// into the client library.
	// For some newer versions, a NULL host causes problems, but an empty
	// string is safe.
#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	const char	*hostval=(host && host[0])?host:"";
#else
	const char	*hostval=(host && host[0])?host:NULL;
#endif

	// Handle db.
	const char	*dbval=(db && db[0])?db:"";
	
	// log in
	const char	*user=getUser();
	const char	*password=getPassword();
#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	// Handle port and socket.
	int		portval=(port && port[0])?charstring::toLong(port):0;
	const char	*socketval=(socket && socket[0])?socket:NULL;
	#if MYSQL_VERSION_ID>=32200
	// initialize database connection structure
	if (!mysql_init(&mysql)) {
		fprintf(stderr,"mysql_init failed\n");
		return false;
	}
	if (!mysql_real_connect(&mysql,hostval,user,password,dbval,
						portval,socketval,0)) {
	#else
	if (!mysql_real_connect(&mysql,hostval,user,password,
						portval,socketval,0)) {
	#endif
		fprintf(stderr,"mysql_real_connect failed: %s\n",
						mysql_error(&mysql));
#else
	if (!mysql_connect(&mysql,hostval,user,password)) {
		fprintf(stderr,"mysql_connect failed: %s\n",
						mysql_error(&mysql));
#endif
		logOut();
		return false;
	}
#ifdef MYSQL_SELECT_DB
	if (mysql_select_db(&mysql,dbval)) {
		fprintf(stderr,"mysql_select_db failed: %s\n",
						mysql_error(&mysql));
		logOut();
		return false;
	}
#endif
	connected=1;
	return true;
}

#ifdef HAVE_MYSQL_CHANGE_USER
bool mysqlconnection::changeUser(const char *newuser,
					const char *newpassword) {
	return !mysql_change_user(&mysql,newuser,newpassword,
					(char *)((db && db[0])?db:""));
}
#endif

sqlrcursor *mysqlconnection::initCursor() {
	return (sqlrcursor *)new mysqlcursor((sqlrconnection *)this);
}

void mysqlconnection::deleteCursor(sqlrcursor *curs) {
	delete (mysqlcursor *)curs;
}

void mysqlconnection::logOut() {
	connected=0;
	mysql_close(&mysql);
}

#ifdef HAVE_MYSQL_PING
bool mysqlconnection::ping() {
	return (!mysql_ping(&mysql))?true:false;
}
#endif

const char *mysqlconnection::identify() {
	return "mysql";
}

bool mysqlconnection::isTransactional() {
	return false;
}

bool mysqlconnection::autoCommitOn() {
#ifdef HAVE_MYSQL_AUTOCOMMIT
	return !mysql_autocommit(&mysql,true);
#else
	// do nothing
	return true;
#endif
}

bool mysqlconnection::autoCommitOff() {
#ifdef HAVE_MYSQL_AUTOCOMMIT
	return !mysql_autocommit(&mysql,false);
#else
	// do nothing
	return true;
#endif
}

bool mysqlconnection::commit() {
#ifdef HAVE_MYSQL_COMMIT
	return !mysql_commit(&mysql);
#else
	// do nothing
	return true;
#endif
}

bool mysqlconnection::rollback() {
#ifdef HAVE_MYSQL_ROLLBACK
	return !mysql_rollback(&mysql);
#else
	// do nothing
	return true;
#endif
}

mysqlcursor::mysqlcursor(sqlrconnection *conn) : sqlrcursor(conn) {
	mysqlconn=(mysqlconnection *)conn;
	mysqlresult=NULL;

	// SID initialization
	sql_injection_detection_database_init(); 
	sql_injection_detection_parameters(); 
}

bool mysqlcursor::executeQuery(const char *query, long length, bool execute) {

	// initialize counts
	ncols=0;
	nrows=0;

	// initialize result set
	mysqlresult=NULL;

	// fake binds
	stringbuffer	*newquery=fakeInputBinds(query);

	// perform ingress fitering
	sql_injection_detection = false;
	sql_injection_detection = sql_injection_detection_ingress(query);

	// execute the query
	if (newquery && !sql_injection_detection) {

		if ((queryresult=mysql_real_query(&mysqlconn->mysql,
					newquery->getString(),
					strlen(newquery->getString())))) {
			delete newquery;
			return false;
		}
		delete newquery;
	} else {
		if ((queryresult=mysql_real_query(&mysqlconn->mysql,
							query,length))) {
			return false;
		}
	}

        if (!sql_injection_detection) {

		checkForTempTable(query,length);

		// get the affected row count
		affectedrows=mysql_affected_rows(&mysqlconn->mysql);

		// store the result set
		if ((mysqlresult=mysql_store_result(&mysqlconn->mysql))==
						(MYSQL_RES *)NULL) {

			// if there was an error then return failure, otherwise
			// the query must have been some DML or DDL
			char	*err=(char *)mysql_error(&mysqlconn->mysql);
			if (err && err[0]) {
				return false;
			} else {
				return true;
			}
		}

		// perform sid egress filtering

		if (sql_injection_detection_egress()==true) {

			mysqlresult=NULL;

			sql_injection_detection=true;

			ncols=0;
			nrows=0;

			return true;
		}


		// get the column count
		ncols=mysql_num_fields(mysqlresult);

		// get the row count
		nrows=mysql_num_rows(mysqlresult);

	}

	return true;
}

const char *mysqlcursor::getErrorMessage(bool *liveconnection) {

	*liveconnection=true;
	const char	*err=mysql_error(&mysqlconn->mysql);
#if defined(HAVE_MYSQL_CR_SERVER_GONE_ERROR) || \
		defined(HAVE_MYSQL_CR_SERVER_LOST) 
	#ifdef HAVE_MYSQL_CR_SERVER_GONE_ERROR
		if (queryresult==CR_SERVER_GONE_ERROR) {
			*liveconnection=false;
		} else
	#endif
	#ifdef HAVE_MYSQL_CR_SERVER_LOST
		if (queryresult==CR_SERVER_LOST) {
			*liveconnection=false;
		} else
	#endif
#endif
	if (!charstring::compare(err,"") ||
		!charstring::compareIgnoringCase(err,
				"mysql server has gone away") ||
		!charstring::compareIgnoringCase(err,
				"Can't connect to local MySQL",28) ||
		!charstring::compareIgnoringCase(err,
			"Lost connection to MySQL server during query")) {
		*liveconnection=false;
	}
	return err;
}

void mysqlcursor::returnColumnCount() {
	conn->sendColumnCount(ncols);
}

void mysqlcursor::returnRowCounts() {

	// send row counts
	conn->sendRowCounts((long)nrows,(long)affectedrows);
}

void mysqlcursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// for DML or DDL queries, return no column info
	if (!mysqlresult) {
		return;
	}

	// some useful variables
	int	type;
	int	length;

	// position ourselves at the first field
	mysql_field_seek(mysqlresult,0);

	// for each column...
	for (int i=0; i<ncols; i++) {

		// fetch the field
		mysqlfield=mysql_fetch_field(mysqlresult);

		// append column type to the header
		if (mysqlfield->type==FIELD_TYPE_STRING) {
			type=STRING_DATATYPE;
			length=(int)mysqlfield->length;
		} else if (mysqlfield->type==FIELD_TYPE_VAR_STRING) {
			type=CHAR_DATATYPE;
			length=(int)mysqlfield->length+1;
		} else if (mysqlfield->type==FIELD_TYPE_DECIMAL) {
			type=DECIMAL_DATATYPE;
			if (mysqlfield->decimals>0) {
				length=(int)mysqlfield->length+2;
			} else if (mysqlfield->decimals==0) {
				length=(int)mysqlfield->length+1;
			}
			if (mysqlfield->length<mysqlfield->decimals) {
				length=(int)mysqlfield->decimals+2;
			}
		} else if (mysqlfield->type==FIELD_TYPE_TINY) {
			type=TINYINT_DATATYPE;
			length=1;
		} else if (mysqlfield->type==FIELD_TYPE_SHORT) {
			type=SMALLINT_DATATYPE;
			length=2;
		} else if (mysqlfield->type==FIELD_TYPE_LONG) {
			type=INT_DATATYPE;
			length=4;
		} else if (mysqlfield->type==FIELD_TYPE_FLOAT) {
			type=FLOAT_DATATYPE;
			if (mysqlfield->length<=24) {
				length=4;
			} else {
				length=8;
			}
		} else if (mysqlfield->type==FIELD_TYPE_DOUBLE) {
			type=REAL_DATATYPE;
			length=8;
		} else if (mysqlfield->type==FIELD_TYPE_LONGLONG) {
			type=BIGINT_DATATYPE;
			length=8;
		} else if (mysqlfield->type==FIELD_TYPE_INT24) {
			type=MEDIUMINT_DATATYPE;
			length=3;
		} else if (mysqlfield->type==FIELD_TYPE_TIMESTAMP) {
			type=TIMESTAMP_DATATYPE;
			length=4;
		} else if (mysqlfield->type==FIELD_TYPE_DATE) {
			type=DATE_DATATYPE;
			length=3;
		} else if (mysqlfield->type==FIELD_TYPE_TIME) {
			type=TIME_DATATYPE;
			length=3;
		} else if (mysqlfield->type==FIELD_TYPE_DATETIME) {
			type=DATETIME_DATATYPE;
			length=8;
#ifdef HAVE_MYSQL_FIELD_TYPE_YEAR
		} else if (mysqlfield->type==FIELD_TYPE_YEAR) {
			type=YEAR_DATATYPE;
			length=1;
#endif
#ifdef HAVE_MYSQL_FIELD_TYPE_NEWDATE
		} else if (mysqlfield->type==FIELD_TYPE_NEWDATE) {
			type=NEWDATE_DATATYPE;
			length=1;
#endif
		} else if (mysqlfield->type==FIELD_TYPE_NULL) {
			type=NULL_DATATYPE;
#ifdef HAVE_MYSQL_FIELD_TYPE_ENUM
		} else if (mysqlfield->type==FIELD_TYPE_ENUM) {
			type=ENUM_DATATYPE;
			// 1 or 2 bytes delepending on the # of enum values
			// (65535 max)
			length=2;
#endif
#ifdef HAVE_MYSQL_FIELD_TYPE_SET
		} else if (mysqlfield->type==FIELD_TYPE_SET) {
			type=SET_DATATYPE;
			// 1,2,3,4 or 8 bytes depending on the # of
			// members (64 max)
			length=8;
#endif
		// For some reason, tinyblobs, mediumblobs and longblobs
		// all show up as FIELD_TYPE_BLOB despite field types being
		// defined for those types.  tinyblobs have a length
		// of 255 though, so that can be used for something.  medium
		// and long blobs both have the same length though.  Go
		// figure.  Also, the word TEXT and BLOB appear to be
		// interchangable.  We'll use BLOB because it appears to be
		// more standard than TEXT.  I wonder if this will be
		// changed in a future incarnation of mysql.  I also wonder
		// what happens on a 64 bit machine.
		} else if (mysqlfield->type==FIELD_TYPE_TINY_BLOB) {
			type=TINY_BLOB_DATATYPE;
			length=(int)mysqlfield->length+2;
		} else if (mysqlfield->type==FIELD_TYPE_MEDIUM_BLOB) {
			type=MEDIUM_BLOB_DATATYPE;
			length=(int)mysqlfield->length+3;
		} else if (mysqlfield->type==FIELD_TYPE_LONG_BLOB) {
			type=LONG_BLOB_DATATYPE;
			length=(int)mysqlfield->length+4;
		} else if (mysqlfield->type==FIELD_TYPE_BLOB) {
			if ((int)mysqlfield->length==255) {
				type=TINY_BLOB_DATATYPE;
				length=(int)mysqlfield->length+2;
			} else {
				type=BLOB_DATATYPE;
				length=(int)mysqlfield->length+3;
			}
		} else {
			type=UNKNOWN_DATATYPE;
			length=(int)mysqlfield->length;
		}

		// send column definition
		// for mysql, length is actually precision
		conn->sendColumnDefinition(mysqlfield->name,
				charstring::length(mysqlfield->name),
				type,length,
				mysqlfield->length,
				mysqlfield->decimals,
				!(IS_NOT_NULL(mysqlfield->flags)),
				IS_PRI_KEY(mysqlfield->flags),
				mysqlfield->flags&UNIQUE_KEY_FLAG,
				mysqlfield->flags&MULTIPLE_KEY_FLAG,
				mysqlfield->flags&UNSIGNED_FLAG,
				mysqlfield->flags&ZEROFILL_FLAG,
#ifdef BINARY_FLAG
				mysqlfield->flags&BINARY_FLAG,
#else
				0,
#endif
#ifdef AUTO_INCREMENT_FLAG
				mysqlfield->flags&AUTO_INCREMENT_FLAG
#else
				0
#endif
				);
	}
}

bool mysqlcursor::noRowsToReturn() {
	// for DML or DDL queries, return no data
	return (!mysqlresult);
}

bool mysqlcursor::skipRow() {
	return fetchRow();
}

bool mysqlcursor::fetchRow() {
	return ((mysqlrow=mysql_fetch_row(mysqlresult))!=NULL);
}

void mysqlcursor::returnRow() {

	for (int col=0; col<ncols; col++) {

		if (mysqlrow[col]) {
			conn->sendField(mysqlrow[col],
					charstring::length(mysqlrow[col]));
		} else {
			conn->sendNullField();
		}
	}
}

void mysqlcursor::cleanUpData(bool freeresult, bool freebinds) {
	if (freeresult && mysqlresult!=(MYSQL_RES *)NULL) {
		mysql_free_result(mysqlresult);
		mysqlresult=NULL;
	}
}

// This method performs SQL Injection Detection
// for ingress filtering

bool mysqlcursor::sql_injection_detection_ingress(const char *query) {
       
	bool ret_val = false;	// flag indicating a sql injection attack


	// Check for possible buffer overfow attack

	if ( strlen(query) > ( BUFSIZ - 1000 ) )
	{

	   sql_injection_detection_log(query,
                              "BO Attack", "Buffer Overflow Attack");
           return true; 

	}

        // parse the SQL statement
	// it will be needed for all modes of operation

        sql_injection_detection_parse_sql(query);

	// if in listening mode, record the sql statement passing thru

        if ( listen_mode == 1 ) {

		sprintf(sid_log_message, "Ingress Listening");

		sql_injection_detection_log(query,
                                            sid_parsed_sql, sid_log_message);

	}

        // if perform ingress filtering,
        // check black list
	// check white list
	// check learning database

        if ( ingress_mode == 1 ) {

           // determine if SQL statement is in black list

           if ( sql_injection_detection_ingress_bl(query) == true )
           {
             ret_val = true;
           }

           // determine if SQL statement is in white list

           else if ( sql_injection_detection_ingress_wl(query) == true )
           {
              ret_val = false;
           }
           else  // determine if SQL statement is in learning database
           {
		if ( sql_injection_detection_ingress_ldb() == false )
                  ret_val = true;
		else
		  ret_val = false;
	   }

	   // verification mode set
           // do not send sql statements to the database for processing

	   if ( verification_mode == 1 )
	   {
              return true;
           }

           // prevention mode set

           if ( prevention_mode == 1 )
           {
              return ret_val;
           }
	
	} 

	return ret_val; 
}

// This method determines if the parsed sql statement
// is in the learning database

bool mysqlcursor::sql_injection_detection_ingress_ldb() {

	bool ret_val = false;

	sprintf(sid_log_message,"Ingress_Ldb_Not_Found");

	if ( sql_injection_detection_check_db("sidingressldb") )
	{

		sprintf(sid_log_message, "Ingress_Ldb_Found");
		ret_val = true;
	}	

	sql_injection_detection_log(sid_parsed_sql,
                                    sid_parsed_sql, sid_log_message);

	return( ret_val );

}

// This method determines if the SQL statement contains
// anything in the white list

bool mysqlcursor::sql_injection_detection_ingress_wl(const char *query) {

	bool ret_val = false;

	sprintf(sid_log_message,"Ingress_Wl_Not_Found");

	if ( sql_injection_detection_check_db("sidingresswlist") )
	{

		sprintf(sid_log_message, "Ingress_Wl_Found");
		ret_val = true;

	}	

	sql_injection_detection_log(query,
                                    sid_parsed_sql, sid_log_message);


	return( ret_val );

}

// This method determines if the SQL statement contains
// anything in the black list

bool mysqlcursor::sql_injection_detection_ingress_bl(const char *query) {

	bool ret_val = false;

	sprintf(sid_log_message, "Ingress_Bl_Not_Found");

	if ( sql_injection_detection_check_db("sidingressblist") )
	{

		sprintf(sid_log_message, "Ingress_Bl_Found");
		ret_val = true;

	}	
	
	sql_injection_detection_log(query,
                                    sid_parsed_sql, sid_log_message);

	return( ret_val );

}

// This method determines if a value is in a database
// It checks both ingress/egress databases
// True - sql statement found in the sid db
// False - sql statement not found in the sid db

bool mysqlcursor::sql_injection_detection_check_db(const char *sid_db) {

	bool ret_val = false;
	bool valid_list = false;
	int i, target_size;
	char target_sql[BUFSIZ];

	// determine what list to search for

	if ( !strncmp(sid_db, "sidingressblist", 15) )
	{

		sprintf(sid_query, "select sql_state from sidingressblist where sql_state =  \'");

		sprintf(target_sql, "%s", sid_parsed_sql);

		valid_list = true;

	}

	if ( !strncmp(sid_db, "sidingresswlist", 15) )
	{

		sprintf(sid_query, "select sql_state from sidingresswlist where sql_state =  \'");

		sprintf(target_sql, "%s", sid_parsed_sql);

		valid_list = true;

	}

	if ( !strncmp(sid_db, "sidegressblist", 14) )
	{

		sprintf(sid_query, "select sql_state from sidegressblist where sql_state =  \'");

		sprintf(target_sql, "%s", sid_parsed_results);

		valid_list = true;

	}

	if ( !strncmp(sid_db, "sidegresswlist", 14) )
	{

		sprintf(sid_query, "select sql_state from sidegresswlist where sql_state =  \'");

		sprintf(target_sql, "%s", sid_parsed_results);

		valid_list = true;

	}

	if ( !strncmp(sid_db, "sidingressldb", 13) )
	{

		sprintf(sid_query, "select parsed_sql from sidingressldb where parsed_sql =  \'");


		sprintf(target_sql, "%s", sid_parsed_sql);

		valid_list = true;

	}

	if ( !strncmp(sid_db, "sidegressldb", 12) )
	{

		sprintf(sid_query, "select parsed_sql from sidegressldb where parsed_sql =  \'");

		sprintf(target_sql, "%s", sid_parsed_results);

		valid_list = true;

	}

	// determine if a valid list was provided

	if ( valid_list ) 
	{

		// Assign the sql_state target value in the
		// sid query by assigning each and every
		// character except the null character
		// The mysql C API functions do not deal
		// well with the null character

		for ( i = 0, 
	      		target_size = strlen(target_sql);
              		i < target_size ; i++ )
		{
			sprintf(sid_query, "%s%c", sid_query, target_sql[i]);
		}

		sprintf(sid_query, "%s\'", sid_query);

		mysql_real_query(sid_mysql, sid_query, (unsigned int)strlen(sid_query));

		sid_res =  mysql_store_result(sid_mysql);

		// if rows returned for sql statement
		// then target sql found

		if ( mysql_num_rows( sid_res ) ) 
		{
			ret_val = true;
		} 

	}

	return ret_val;

}

// This method initializes the database connection
// for interaction between SID and its tables

void mysqlcursor::sql_injection_detection_database_init() {

	sid_mysql = mysql_init(NULL);

	if ( !mysql_real_connect(sid_mysql, "localhost", "siduser",
				 "hapk1do", "siddb", 0, NULL, 0) )
        {
		exit(30);
        }

}

// This method gets the parameters SID needs to 
// determine what to do

void mysqlcursor::sql_injection_detection_parameters() {

	unsigned long num_rows = 0;
	unsigned long m_num_rows;
	int param_value = 0;

	sprintf(sid_query, "select UPPER(sparam), svalue from sidparameters");

	mysql_real_query(sid_mysql, sid_query, strlen(sid_query));

	sid_res = mysql_store_result(sid_mysql);
	m_num_rows = mysql_num_rows(sid_res);

	ingress_mode = 0;
	egress_mode = 0;
	listen_mode = 0;
	verification_mode = 0;
	prevention_mode = 0;

	for (num_rows = 0; num_rows < m_num_rows; num_rows++)
	{
	
		sid_row = mysql_fetch_row(sid_res);

		param_value = atoi(sid_row[1]);

		if ( strcmp(sid_row[0], "INGRESS") == 0 )
		   ingress_mode = param_value;

		if ( strcmp(sid_row[0], "EGRESS") == 0 )
		   egress_mode = param_value;

		if ( strcmp(sid_row[0], "LISTEN") == 0 )
		   listen_mode = param_value;

		if ( strcmp(sid_row[0], "VERIFICATION") == 0 )
		   verification_mode = param_value;
 
		if ( strcmp(sid_row[0], "PREVENTION") == 0 )
		   prevention_mode = param_value;

	}

}

// This method maintains a log for SID
// The SQL statement, parsed sql and message will be logged

void mysqlcursor::sql_injection_detection_log(const char *query, 
					      char *parsed_sql,
					      char *log_message) {
	int i, buf_len;

	char param1[BUFSIZ], param2[BUFSIZ], param3[BUFSIZ];

	sprintf(param1, "%s", query);
	sprintf(param2, "%s", parsed_sql);
	sprintf(param3, "%s", log_message);

	sprintf(sid_query, "insert into sidlog (sql_state, parsed_sql, sid_flag) values (\"");

	for ( i = 0, buf_len = strlen(param1); i < buf_len; i++ )
	{
	   if ( param1[i] != '\0' )
	      sprintf(sid_query, "%s%c", sid_query, param1[i]);
	} 

	sprintf(sid_query, "%s\", \"", sid_query);

	for ( i = 0, buf_len = strlen(param2); i < buf_len; i++ )
	{
	   if ( param2[i] != '\0' )
	      sprintf(sid_query, "%s%c", sid_query, param2[i]);
	} 

	sprintf(sid_query, "%s\", \"", sid_query);

	for ( i = 0, buf_len = strlen(param3); i < buf_len; i++ )
	{
	   if ( param3[i] != '\0' )
	      sprintf(sid_query, "%s%c", sid_query, param3[i]);
	} 

	sprintf(sid_query, "%s\")", sid_query);

	sid_query_result = mysql_real_query(sid_mysql, sid_query, strlen(sid_query));

}

// This method parses the SQL statement and represents
// it in a format for the learning database

void mysqlcursor::sql_injection_detection_parse_sql(const char *query) {

	unsigned long  temp_querylength;
	int quote_count = 0;
	unsigned long i;
	unsigned long j;
	bool equal_sign;

	equal_sign = false;

	i = 0;
	j = 0;

	temp_querylength = strlen(query);

        for ( i = 0; i < BUFSIZ; i++ )
	{
		sid_parsed_sql[i] = '\0';
	}


        for ( i = 0; i < temp_querylength; i++ )
	{
           if ( query[i] == '\'' )
	   {
	      quote_count = quote_count + 1; 

	      if ( quote_count == 2 )
                 quote_count = 0;
	   } 
	   else if ( ( query[i] != ' ' ) && ( query[i] != ',' ) )
           {
              if (  quote_count == 0 )
	      {
		 if ( j < BUFSIZ )
		 {
                    sid_parsed_sql[j] = query[i];
		 }
                 j = j + 1;

		 if ( query[i] == '=' )
		 {

			while ( (query[i + 1] == ' ') && ((i + 1) < BUFSIZ) ) 
			   i++;	

			while ( (isdigit(query[i + 1])) && ((i + 1) < BUFSIZ))
			  i++;

		 }
              } 
           }
	}

	sid_parsed_sql[j] = '\0';

}

// This method performs egress filtering for SID

bool mysqlcursor::sql_injection_detection_egress() {
       
	bool ret_val = false;	// flag indicating a sql injection attack

        // parse the SQL results 
	// it will be needed for all modes of operation

        sql_injection_detection_parse_results();

	// Detect if a buffer overflow attack

	if ( strlen(sid_parsed_results) > ( BUFSIZ - 1000 ) )
	{

		sql_injection_detection_log("BO Attack", sid_parsed_results,
                                            "Buffer Overflow Attack");
		
		return false;
	}

	// if in listening mode, record the sql resuts passing thru

        if ( listen_mode == 1 ) {

		sprintf(sid_log_message, "Egress Listening");

		sql_injection_detection_log(sid_log_message, sid_parsed_results,
                                            sid_log_message);


	}

        // if perform egress filtering,
        // check black list
	// check white list
	// check learning database

        if ( egress_mode == 1 ) {

           // determine if SQL statement is in black list

           if ( sql_injection_detection_egress_bl() == true )
           {
             ret_val = true;
           }

           // determine if SQL statement is in white list

           else if ( sql_injection_detection_egress_wl() == true )
           {
              ret_val = false;
           }
           else  // determine if SQL statement is in learning database
           {
		if ( sql_injection_detection_egress_ldb() == false )
                  ret_val = true;
	   }

	   // Verification mode is not an option when
	   // analyzing the result set due to the fact
           // that the SQL statement has already processed

           // prevention mode set

           if ( prevention_mode == 1 )
           {
              return ret_val;
           }
	
	} 

	return false; 
}

// This method parses the SQL results

void mysqlcursor::sql_injection_detection_parse_results() {

	int num_fields;
	int i;

	for ( i = 0; i < BUFSIZ; i++ )
	{
		sid_parsed_results[i] = '\0';
	}

	num_fields = mysql_num_fields(mysqlresult);

	sid_fields = mysql_fetch_fields(mysqlresult);	

	for (i=0; i < num_fields; i++)
	{

		if ( ( strlen(sid_parsed_results) +
		     strlen(sid_fields[i].name) ) < BUFSIZ )

			sprintf(sid_parsed_results, "%s%s",
			sid_parsed_results, sid_fields[i].name);

	}
	
}

// This method performs egress black list processing

bool mysqlcursor::sql_injection_detection_egress_bl() {

	bool ret_val = false;

	sprintf(sid_log_message, "Egress_Bl_Not_Found");

	if ( sql_injection_detection_check_db("sidegressblist") )
	{

		sprintf(sid_log_message, "Egress_Bl_Found");
		ret_val = true;

	}	
	
	sql_injection_detection_log(sid_log_message,
				    sid_parsed_results, sid_log_message);


	return( ret_val );

}

// This method performs egress white list processing

bool mysqlcursor::sql_injection_detection_egress_wl() {

	bool ret_val = false;

	sprintf(sid_log_message,"Egress_Wl_Not_Found");

	if ( sql_injection_detection_check_db("sidegresswlist") )
	{

		sprintf(sid_log_message, "Egress_Wl_Found");
		ret_val = true;

	}	
	
	sql_injection_detection_log(sid_log_message, 
                                    sid_parsed_results, sid_log_message);

	return( ret_val );

}

// This method performs egress filtering for learning db processing

bool mysqlcursor::sql_injection_detection_egress_ldb() {

	bool ret_val = false;

	sprintf(sid_log_message,"Egress_Ldb_Not_Found");

	if ( sql_injection_detection_check_db("sidegressldb") )
	{

		sprintf(sid_log_message, "Egress_Ldb_Found");
		ret_val = true;

	}	

	sql_injection_detection_log(sid_log_message, 
                                    sid_parsed_results, sid_log_message);

	return( ret_val );

}
