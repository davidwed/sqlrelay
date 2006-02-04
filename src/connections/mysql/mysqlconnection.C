// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>
#include <mysqlconnection.h>
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID>=32200
	#include <errmsg.h>
#endif

#include <datatypes.h>

#include <config.h>

#include <stdlib.h>

mysqlconnection::mysqlconnection() : sqlrconnection_svr() {
	connected=false;
#ifdef HAVE_MYSQL_STMT_PREPARE
	fakebinds=false;
#endif
}

uint16_t mysqlconnection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

bool mysqlconnection::supportsNativeBinds() {
#ifdef HAVE_MYSQL_STMT_PREPARE
	return !fakebinds;
#else
	return false;
#endif
}

void mysqlconnection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	db=connectStringValue("db");
	host=connectStringValue("host");
	port=connectStringValue("port");
	socket=connectStringValue("socket");
#ifdef HAVE_MYSQL_STMT_PREPARE
	fakebinds=!charstring::compare(connectStringValue("fakebinds"),"yes");
#endif
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
	int		portval=(port && port[0])?charstring::toInteger(port):0;
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
	connected=true;
	return true;
}

#ifdef HAVE_MYSQL_CHANGE_USER
bool mysqlconnection::changeUser(const char *newuser,
					const char *newpassword) {
	return !mysql_change_user(&mysql,newuser,newpassword,
					(char *)((db && db[0])?db:""));
}
#endif

sqlrcursor_svr *mysqlconnection::initCursor() {
	return (sqlrcursor_svr *)new mysqlcursor((sqlrconnection_svr *)this);
}

void mysqlconnection::deleteCursor(sqlrcursor_svr *curs) {
	delete (mysqlcursor *)curs;
}

void mysqlconnection::logOut() {
	connected=false;
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

#ifdef HAVE_MYSQL_STMT_PREPARE
short mysqlconnection::nonNullBindValue() {
	return 0;
}

short mysqlconnection::nullBindValue() {
	return 1;
}
#endif

mysqlcursor::mysqlcursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {
	mysqlconn=(mysqlconnection *)conn;
	mysqlresult=NULL;
	columnnames=NULL;
#ifdef HAVE_MYSQL_STMT_PREPARE
	stmt=mysql_stmt_init(&mysqlconn->mysql);
	for (unsigned short index=0; index<MAX_SELECT_LIST_SIZE; index++) {
		fieldbind[index].buffer_type=MYSQL_TYPE_STRING;
		fieldbind[index].buffer=(char *)&field[index];
		fieldbind[index].buffer_length=MAX_ITEM_BUFFER_SIZE;
		fieldbind[index].is_null=&isnull[index];
		fieldbind[index].length=&fieldlength[index];
	}
#endif
}

mysqlcursor::~mysqlcursor() {
	delete[] columnnames;
#ifdef HAVE_MYSQL_STMT_PREPARE
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
	if (mysqlresult) {
		mysql_free_result(mysqlresult);
	}
#endif
}

#ifdef HAVE_MYSQL_STMT_PREPARE
bool mysqlcursor::prepareQuery(const char *query, uint32_t length) {

	if (mysqlconn->fakebinds) {
		return true;
	}

	// store inbindcount here, otherwise if rebinding/reexecution occurs and
	// the client tries to bind more variables than were defined when the
	// query was prepared, it would cause the inputBind methods to attempt
	// to address beyond the end of the various arrays
	bindcount=inbindcount;

	// reset bind counter
	bindcounter=0;

	return !mysql_stmt_prepare(stmt,query,length);
}

bool mysqlcursor::inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						int16_t *isnull) {
printf("inputBindString...\n");

	if (mysqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=valuesize;

	rawbuffer::zero(&bind[bindcounter],sizeof(MYSQL_BIND));
	if (*isnull) {
printf("is null\n");
		bind[bindcounter].buffer_type=MYSQL_TYPE_NULL;
		bind[bindcounter].buffer=(void *)NULL;
		bind[bindcounter].buffer_length=0;
		bind[bindcounter].length=0;
	} else {
printf("not null\n");
		bind[bindcounter].buffer_type=MYSQL_TYPE_STRING;
		bind[bindcounter].buffer=(void *)value;
		bind[bindcounter].buffer_length=valuesize;
		bind[bindcounter].length=&bindvaluesize[bindcounter];
	}
	bind[bindcounter].is_null=(my_bool *)isnull;

	return !mysql_stmt_bind_param(stmt,&bind[bindcounter++]);
}

bool mysqlcursor::inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value) {
printf("inputBindInteger...\n");

	if (mysqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=sizeof(int64_t);

	rawbuffer::zero(&bind[bindcounter],sizeof(MYSQL_BIND));
	if (*isnull) {
printf("is null\n");
		bind[bindcounter].buffer_type=MYSQL_TYPE_NULL;
		bind[bindcounter].buffer=(void *)NULL;
		bind[bindcounter].buffer_length=0;
		bind[bindcounter].length=0;
	} else {
printf("not null\n");
		bind[bindcounter].buffer_type=MYSQL_TYPE_LONGLONG;
		bind[bindcounter].buffer=(void *)value;
		bind[bindcounter].buffer_length=sizeof(int64_t);
		bind[bindcounter].length=&bindvaluesize[bindcounter];
	}
	bind[bindcounter].is_null=(my_bool *)isnull;

	return !mysql_stmt_bind_param(stmt,&bind[bindcounter++]);
}

bool mysqlcursor::inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale) {
printf("inputBindDouble...\n");

	if (mysqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=sizeof(double);

	rawbuffer::zero(&bind[bindcounter],sizeof(MYSQL_BIND));
	if (*isnull) {
		bind[bindcounter].buffer_type=MYSQL_TYPE_NULL;
		bind[bindcounter].buffer=(void *)NULL;
		bind[bindcounter].buffer_length=0;
		bind[bindcounter].length=0;
	} else {
		bind[bindcounter].buffer_type=MYSQL_TYPE_DOUBLE;
		bind[bindcounter].buffer=(void *)value;
		bind[bindcounter].buffer_length=sizeof(double);
		bind[bindcounter].length=&bindvaluesize[bindcounter];
	}
	bind[bindcounter].is_null=(my_bool *)isnull;

	return !mysql_stmt_bind_param(stmt,&bind[bindcounter++]);
}

bool mysqlcursor::inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
printf("inputBindBlob...\n");

	if (mysqlconn->fakebinds) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=valuesize;

	rawbuffer::zero(&bind[bindcounter],sizeof(MYSQL_BIND));
	if (*isnull) {
		bind[bindcounter].buffer_type=MYSQL_TYPE_NULL;
		bind[bindcounter].buffer=(void *)NULL;
		bind[bindcounter].buffer_length=0;
		bind[bindcounter].length=0;
	} else {
		bind[bindcounter].buffer_type=MYSQL_TYPE_LONG_BLOB;
		bind[bindcounter].buffer=(void *)value;
		bind[bindcounter].buffer_length=valuesize;
		bind[bindcounter].length=&bindvaluesize[bindcounter];
	}
	bind[bindcounter].is_null=(my_bool *)isnull;

	return !mysql_stmt_bind_param(stmt,&bind[bindcounter++]);
}

bool mysqlcursor::inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
printf("inputBindClob...\n");
	return inputBindBlob(variable,variablesize,value,valuesize,isnull);
}
#endif

bool mysqlcursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	// initialize counts
	ncols=0;
	nrows=0;

#ifdef HAVE_MYSQL_STMT_PREPARE

	// prepare the query if necessary
	if (mysqlconn->fakebinds) {
		if (mysql_stmt_prepare(stmt,query,length)) {
			return false;
		}
	}

	// execute the query
	if ((queryresult=mysql_stmt_execute(stmt))) {
		return false;
	}

	checkForTempTable(query,length);
	
	// get the affected row count
	affectedrows=mysql_stmt_affected_rows(stmt);

	// get the column count
	ncols=mysql_stmt_field_count(stmt);

	// get the metadata
	mysqlresult=NULL;
	if (ncols) {
		mysqlresult=mysql_stmt_result_metadata(stmt);
	}

	// bind the fields
	if (ncols && mysql_stmt_bind_result(stmt,fieldbind)) {
		return false;
	}

	// get the row count
	nrows=mysql_stmt_num_rows(stmt);

	// store the result set
	if (nrows && mysql_stmt_store_result(stmt)) {
		return false;
	}

#else

	// initialize result set
	mysqlresult=NULL;

	// execute the query
	if ((queryresult=mysql_real_query(&mysqlconn->mysql,query,length))) {
		return false;
	}

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

	// get the column count
	ncols=mysql_num_fields(mysqlresult);

	// get the row count
	nrows=mysql_num_rows(mysqlresult);
#endif

	return true;
}

const char *mysqlcursor::errorMessage(bool *liveconnection) {

	*liveconnection=true;
#ifdef HAVE_MYSQL_STMT_PREPARE
	const char	*err=mysql_stmt_error(stmt);
#else
	const char	*err=mysql_error(&mysqlconn->mysql);
#endif
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

uint32_t mysqlcursor::colCount() {
	return ncols;
}

const char * const * mysqlcursor::columnNames() {
	mysql_field_seek(mysqlresult,0);
	columnnames=new char *[ncols];
	for (unsigned int i=0; i<ncols; i++) {
		columnnames[i]=mysql_fetch_field(mysqlresult)->name;
	}
	return columnnames;
}

bool mysqlcursor::knowsRowCount() {
	return true;
}

uint64_t mysqlcursor::rowCount() {
	return nrows;
}

bool mysqlcursor::knowsAffectedRows() {
	return true;
}

uint64_t mysqlcursor::affectedRows() {
	return affectedrows;
}

uint16_t mysqlcursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_IDS;
}

void mysqlcursor::returnColumnInfo() {

	// for DML or DDL queries, return no column info
	if (!mysqlresult) {
		return;
	}

	// some useful variables
	uint16_t	type;
	uint32_t	length;

	// position ourselves at the first field
	mysql_field_seek(mysqlresult,0);

	// for each column...
	for (unsigned int i=0; i<ncols; i++) {

		// fetch the field
		mysqlfield=mysql_fetch_field(mysqlresult);

		// append column type to the header
		if (mysqlfield->type==FIELD_TYPE_STRING) {
			type=STRING_DATATYPE;
			length=(uint32_t)mysqlfield->length;
		} else if (mysqlfield->type==FIELD_TYPE_VAR_STRING) {
			type=CHAR_DATATYPE;
			length=(uint32_t)mysqlfield->length+1;
		} else if (mysqlfield->type==FIELD_TYPE_DECIMAL) {
			type=DECIMAL_DATATYPE;
			if (mysqlfield->decimals>0) {
				length=(uint32_t)mysqlfield->length+2;
			} else if (mysqlfield->decimals==0) {
				length=(uint32_t)mysqlfield->length+1;
			}
			if (mysqlfield->length<mysqlfield->decimals) {
				length=(uint32_t)mysqlfield->decimals+2;
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
		// For some versions of mysql, tinyblobs, mediumblobs and
		// longblobs all show up as FIELD_TYPE_BLOB despite field types
		// being defined for those types.  tinyblobs have a length
		// of 255 though, so that can be used for something.  medium
		// and long blobs both have the same length though.  Go
		// figure.  Also, the word TEXT and BLOB appear to be
		// interchangable.  We'll use BLOB because it appears to be
		// more standard than TEXT.  I wonder if this will be
		// changed in a future incarnation of mysql.  I also wonder
		// what happens on a 64 bit machine.
		} else if (mysqlfield->type==FIELD_TYPE_TINY_BLOB ||
				(mysqlfield->type==FIELD_TYPE_BLOB &&
						mysqlfield->length<256)) {
			type=TINY_BLOB_DATATYPE;
			length=255;
		} else if (mysqlfield->type==FIELD_TYPE_BLOB &&
						mysqlfield->length<65536) {
			type=BLOB_DATATYPE;
			length=65535;
		} else if (mysqlfield->type==FIELD_TYPE_MEDIUM_BLOB ||
				(mysqlfield->type==FIELD_TYPE_BLOB &&
						mysqlfield->length<16777216)) {
			type=MEDIUM_BLOB_DATATYPE;
			length=16777215;
		} else if (mysqlfield->type==FIELD_TYPE_LONG_BLOB ||
					mysqlfield->type==FIELD_TYPE_BLOB) {
			type=LONG_BLOB_DATATYPE;
			length=2147483647;
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
#ifdef HAVE_MYSQL_STMT_PREPARE
	return !mysql_stmt_fetch(stmt);
#else
	return ((mysqlrow=mysql_fetch_row(mysqlresult))!=NULL);
#endif
}

void mysqlcursor::returnRow() {

	for (unsigned int col=0; col<ncols; col++) {
#ifdef HAVE_MYSQL_STMT_PREPARE
		if (!isnull[col]) {
			conn->sendField(field[col],fieldlength[col]);
		} else {
			conn->sendNullField();
		}
#else
		if (mysqlrow[col]) {
			conn->sendField(mysqlrow[col],
					charstring::length(mysqlrow[col]));
		} else {
			conn->sendNullField();
		}
#endif
	}
}

void mysqlcursor::cleanUpData(bool freeresult, bool freebinds) {
#ifdef HAVE_MYSQL_STMT_PREPARE
	if (freebinds) {
		mysql_stmt_reset(stmt);
	}
	if (freeresult) {
		mysql_stmt_free_result(stmt);
		if (mysqlresult) {
			mysql_free_result(mysqlresult);
			mysqlresult=NULL;
		}
	}
#else
	if (freeresult && mysqlresult!=(MYSQL_RES *)NULL) {
		mysql_free_result(mysqlresult);
		mysqlresult=NULL;
	}
#endif
	delete[] columnnames;
	columnnames=NULL;
}
