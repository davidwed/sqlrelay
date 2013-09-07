// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <mysqlsqlwriter.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/regularexpression.h>
#include <rudiments/null.h>

#include <datatypes.h>
#include <config.h>

#include <mysql.h>
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID>=32200
	#include <errmsg.h>
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#define MAX_SELECT_LIST_SIZE	256
#ifdef HAVE_MYSQL_STMT_PREPARE
	#define MAX_ITEM_BUFFER_SIZE	32768
#endif

class mysqlconnection;

class mysqlcursor : public sqlrcursor_svr {
	friend class mysqlconnection;
	private:
				mysqlcursor(sqlrconnection_svr *conn);
				~mysqlcursor();
#ifdef HAVE_MYSQL_STMT_PREPARE
		bool		open(uint16_t id);
		bool		close();
		bool		prepareQuery(const char *query,
						uint32_t length);
#endif
		bool		supportsNativeBinds();
#ifdef HAVE_MYSQL_STMT_PREPARE
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		bool		inputBind(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
#endif
		bool		executeQuery(const char *query,
						uint32_t length);
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		uint16_t	getColumnIsNullable(uint32_t col);
		uint16_t	getColumnIsPrimaryKey(uint32_t col);
		uint16_t	getColumnIsUnique(uint32_t col);
		uint16_t	getColumnIsPartOfKey(uint32_t col);
		uint16_t	getColumnIsUnsigned(uint32_t col);
		uint16_t	getColumnIsZeroFilled(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		uint16_t	getColumnIsAutoIncrement(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		cleanUpData();

		MYSQL_RES	*mysqlresult;
		MYSQL_FIELD	*mysqlfields[MAX_SELECT_LIST_SIZE];
		uint32_t	mysqlfieldindex;
		unsigned int	ncols;
		my_ulonglong	nrows;
		my_ulonglong	affectedrows;
		int		queryresult;

#ifdef HAVE_MYSQL_STMT_PREPARE
		MYSQL_STMT	*stmt;
		bool		stmtfreeresult;

		MYSQL_BIND	fieldbind[MAX_SELECT_LIST_SIZE];
		char		field[MAX_SELECT_LIST_SIZE]
					[MAX_ITEM_BUFFER_SIZE];
		my_bool		isnull[MAX_SELECT_LIST_SIZE];
		unsigned long	fieldlength[MAX_SELECT_LIST_SIZE];

		int		bindcount;
		int		bindcounter;
		MYSQL_BIND	*bind;
		unsigned long	*bindvaluesize;

		bool		usestmtprepare;

		regularexpression	unsupportedbystmt;
#endif
		MYSQL_ROW	mysqlrow;
		unsigned long	*mysqlrowlengths;

		mysqlconnection	*mysqlconn;
};

class mysqlconnection : public sqlrconnection_svr {
	friend class mysqlcursor;
	public:
				mysqlconnection(sqlrcontroller_svr *cont);
				~mysqlconnection();
	private:
		void		handleConnectString();
		bool		logIn(const char **error);
#ifdef HAVE_MYSQL_CHANGE_USER
		bool		changeUser(const char *newuser,
						const char *newpassword);
#endif
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		bool		isTransactional();
#ifdef HAVE_MYSQL_PING
		bool		ping();
#endif
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*bindFormat();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(const char *table,
								bool wild);
		const char	*selectDatabaseQuery();
		const char	*getCurrentDatabaseQuery();
		bool		getLastInsertId(uint64_t *id);
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		sqlwriter	*getSqlWriter();
#ifdef HAVE_MYSQL_STMT_PREPARE
		short		nonNullBindValue();
		short		nullBindValue();
#endif
		void		endSession();

		MYSQL	mysql;
		bool	connected;

		const char	*db;
		const char	*host;
		const char	*port;
		const char	*socket;
		const char	*charset;

		char	*dbversion;
		char	*dbhostname;

		static const my_bool	mytrue;
		static const my_bool	myfalse;

		bool		firstquery;

		stringbuffer	loginerror;
};

extern "C" {
	sqlrconnection_svr *new_mysqlconnection(sqlrcontroller_svr *cont) {
		return new mysqlconnection(cont);
	}
}

const my_bool	mysqlconnection::mytrue=TRUE;
const my_bool	mysqlconnection::myfalse=FALSE;

mysqlconnection::mysqlconnection(sqlrcontroller_svr *cont) :
					sqlrconnection_svr(cont) {
	connected=false;
	dbversion=NULL;
	dbhostname=NULL;

	// start this at false because we don't need to do a commit before
	// the first query when we very first start up
	firstquery=false;
}

mysqlconnection::~mysqlconnection() {
	delete[] dbversion;
	delete[] dbhostname;
}

void mysqlconnection::handleConnectString() {
	cont->setUser(cont->connectStringValue("user"));
	cont->setPassword(cont->connectStringValue("password"));
	db=cont->connectStringValue("db");
	host=cont->connectStringValue("host");
	port=cont->connectStringValue("port");
	socket=cont->connectStringValue("socket");
	charset=cont->connectStringValue("charset");
	cont->fakeinputbinds=!charstring::compare(
				cont->connectStringValue("fakebinds"),"yes");
}

bool mysqlconnection::logIn(const char **error) {

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
	const char	*user=cont->getUser();
	const char	*password=cont->getPassword();
#ifdef HAVE_MYSQL_REAL_CONNECT_FOR_SURE
	// Handle port and socket.
	int		portval=(port && port[0])?charstring::toInteger(port):0;
	const char	*socketval=(socket && socket[0])?socket:NULL;
	unsigned long	clientflag=0;
	#ifdef CLIENT_MULTI_STATEMENTS
	clientflag=CLIENT_MULTI_STATEMENTS;
	#endif
	#if MYSQL_VERSION_ID>=32200
	// initialize database connection structure
	if (!mysql_init(&mysql)) {
		*error="mysql_init failed";
		return false;
	}
	if (!mysql_real_connect(&mysql,hostval,user,password,dbval,
					portval,socketval,clientflag)) {
	#else
	if (!mysql_real_connect(&mysql,hostval,user,password,
					portval,socketval,clientflag)) {
	#endif
		loginerror.clear();
		loginerror.append("mysql_real_connect failed: ");
		loginerror.append(mysql_error(&mysql));
		*error=loginerror.getString();
#else
	if (!mysql_connect(&mysql,hostval,user,password)) {
		loginerror.clear();
		loginerror.append("mysql_connect failed: ");
		loginerror.append(mysql_error(&mysql));
		*error=loginerror.getString();
#endif
		logOut();
		return false;
	}

#ifdef HAVE_MYSQL_OPT_RECONNECT
	// Enable autoreconnect in the C api
	// (ordinarily mysql_options should be called before mysql_connect,
	// but not for this option)
	mysql_options(&mysql,MYSQL_OPT_RECONNECT,&mytrue);
#endif

#ifdef MYSQL_SELECT_DB
	if (mysql_select_db(&mysql,dbval)) {
		loginerror.clear();
		loginerror.append("mysql_select_db failed: ");
		loginerror.append(mysql_error(&mysql));
		*error=loginerror.getString();
		logOut();
		return false;
	}
#endif
	connected=true;

#ifdef HAVE_MYSQL_STMT_PREPARE
	// fake binds when connected to older servers
#ifdef HAVE_MYSQL_GET_SERVER_VERSION
	if (mysql_get_server_version(&mysql)<40102) {
		cont->setFakeInputBinds(true);
	}
#else
	char		**list;
	uint64_t	listlen;
	charstring::split(mysql_get_server_info(&mysql),
				".",true,&list,&listlen);

	if (listlen==3) {
		uint64_t	major=charstring::toUnsignedInteger(list[0]);
		uint64_t	minor=charstring::toUnsignedInteger(list[1]);
		uint64_t	patch=charstring::toUnsignedInteger(list[2]);
		if (major>4 || (major==4 && minor>1) ||
				(major==4 && minor==1 && patch>=2)) {
			setFakeInputBinds(true);
		} 
		for (uint64_t index=0; index<listlen; index++) {
			delete[] list[index];
		}
		delete[] list;
	}
#endif

	// get the db host name
	const char	*hostinfo=mysql_get_host_info(&mysql);
	const char	*space=charstring::findFirst(hostinfo,' ');
	if (space) {
		dbhostname=charstring::duplicate(hostinfo,space-hostinfo);
	} else {
		dbhostname=charstring::duplicate(hostinfo);
	}

#endif

#ifdef HAVE_MYSQL_SET_CHARACTER_SET
	// set the character set
	if (charstring::length(charset)) {
		mysql_set_character_set(&mysql,charset);
	}
#endif

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

const char *mysqlconnection::dbVersion() {
	delete[] dbversion;
	dbversion=charstring::duplicate(mysql_get_server_info(&mysql));
	return dbversion;
}

const char *mysqlconnection::dbHostName() {
	return dbhostname;
}

const char *mysqlconnection::bindFormat() {
#ifdef HAVE_MYSQL_STMT_PREPARE
	return "?";
#else
	return sqlrconnection_svr::bindFormat();
#endif
}

const char *mysqlconnection::getDatabaseListQuery(bool wild) {
	return (wild)?"show databases like '%s'":"show databases";
}

const char *mysqlconnection::getTableListQuery(bool wild) {
	return (wild)?"show tables like '%s'":"show tables";
}

const char *mysqlconnection::getColumnListQuery(const char *table,
								bool wild) {
	return (wild)?"select "
			"	column_name, "
			"	data_type, "
			"	character_maximum_length, "
			"	numeric_precision, "
			"	numeric_scale, "
			"	is_nullable, "
			"	column_key, "
			"	column_default, "
			"	extra "
			"from "
			"	information_schema.columns "
			"where "
			"	table_name='%s' "
			"	and "
			"	column_name like '%s'"
			:
			"select "
			"	column_name, "
			"	data_type, "
			"	character_maximum_length, "
			"	numeric_precision, "
			"	numeric_scale, "
			"	is_nullable, "
			"	column_key, "
			"	column_default, "
			"	extra "
			"from "
			"	information_schema.columns "
			"where "
			"	table_name='%s' ";
}

const char *mysqlconnection::selectDatabaseQuery() {
	return "use %s";
}

const char *mysqlconnection::getCurrentDatabaseQuery() {
	return "select database()";
}

bool mysqlconnection::getLastInsertId(uint64_t *id) {
	*id=mysql_insert_id(&mysql);
	return true;
}

bool mysqlconnection::isTransactional() {
	return true;
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

void mysqlconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	const char	*errorstring=mysql_error(&mysql);
	*errorlength=charstring::length(errorstring);
	charstring::safeCopy(errorbuffer,errorbufferlength,
					errorstring,*errorlength);
	*errorcode=mysql_errno(&mysql);
	*liveconnection=(!charstring::compare(errorstring,"") ||
		!charstring::compareIgnoringCase(errorstring,
				"mysql server has gone away") ||
		!charstring::compareIgnoringCase(errorstring,
				"Can't connect to local MySQL",28) /*||
		!charstring::compareIgnoringCase(errorstring,
			"Lost connection to MySQL server during query")*/);
}

sqlwriter *mysqlconnection::getSqlWriter() {
	return new mysqlsqlwriter;
}

#ifdef HAVE_MYSQL_STMT_PREPARE
short mysqlconnection::nonNullBindValue() {
	return 0;
}

short mysqlconnection::nullBindValue() {
	return 1;
}
#endif

void mysqlconnection::endSession() {
	firstquery=true;
}

mysqlcursor::mysqlcursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {
	mysqlconn=(mysqlconnection *)conn;
	mysqlresult=NULL;
#ifdef HAVE_MYSQL_STMT_PREPARE
	stmt=NULL;
	stmtfreeresult=false;
	bind=new MYSQL_BIND[conn->cont->maxbindcount];
	bindvaluesize=new unsigned long[conn->cont->maxbindcount];
	usestmtprepare=true;
	unsupportedbystmt.compile(
			"^\\s*((create|CREATE|drop|DROP|procedure|PROCEDURE|function|FUNCTION|use|USE|CALL|call|START|start)\\s+)|((begin|BEGIN)\\s*)");
	unsupportedbystmt.study();
	for (unsigned short index=0; index<MAX_SELECT_LIST_SIZE; index++) {
		mysqlfields[index]=NULL;
		fieldbind[index].buffer_type=MYSQL_TYPE_STRING;
		fieldbind[index].buffer=(char *)&field[index];
		fieldbind[index].buffer_length=MAX_ITEM_BUFFER_SIZE;
		fieldbind[index].is_null=&isnull[index];
		fieldbind[index].length=&fieldlength[index];
	}
#endif
}

mysqlcursor::~mysqlcursor() {
#ifdef HAVE_MYSQL_STMT_PREPARE
	if (stmtfreeresult) {
		mysql_stmt_free_result(stmt);
	}
	if (mysqlresult) {
		mysql_free_result(mysqlresult);
	}
	delete[] bind;
	delete[] bindvaluesize;
#endif
}

#ifdef HAVE_MYSQL_STMT_PREPARE
bool mysqlcursor::open(uint16_t id) {
	stmt=mysql_stmt_init(&mysqlconn->mysql);
	return true;
}
#endif

#ifdef HAVE_MYSQL_STMT_PREPARE
bool mysqlcursor::close() {
	mysql_stmt_close(stmt);
	return true;
}
#endif

#ifdef HAVE_MYSQL_STMT_PREPARE
bool mysqlcursor::prepareQuery(const char *query, uint32_t length) {

	// if this if the first query of the session, do a commit first,
	// doing this will refresh this connection with any data committed
	// by other connections, which is what would happen if a new client
	// connected directly to mysql
	if (mysqlconn->firstquery) {
		mysqlconn->commit();
		mysqlconn->firstquery=false;
	}

	// can't use stmt API to run a couple of types of queries as of 5.0
	usestmtprepare=true;
	if (unsupportedbystmt.match(query)) {
		usestmtprepare=false;
		return true;
	}

	// store inbindcount here, otherwise if rebinding/reexecution occurs and
	// the client tries to bind more variables than were defined when the
	// query was prepared, it would cause the inputBind methods to attempt
	// to address beyond the end of the various arrays
	bindcount=inbindcount;

	// reset bind counter
	bindcounter=0;

	// re-init bind buffers
	for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
		rawbuffer::zero(&bind[i],sizeof(MYSQL_BIND));
	}

	// prepare the statement
	if (!mysql_stmt_prepare(stmt,query,length)) {
		stmtfreeresult=true;
		return true;
	}
	return false;
}
#endif

bool mysqlcursor::supportsNativeBinds() {
#ifdef HAVE_MYSQL_STMT_PREPARE
	return (usestmtprepare);
#else
	return false;
#endif
}

#ifdef HAVE_MYSQL_STMT_PREPARE
bool mysqlcursor::inputBind(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {

	if (!usestmtprepare) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=valuesize;

	if (*isnull) {
		bind[bindcounter].buffer_type=MYSQL_TYPE_NULL;
		bind[bindcounter].buffer=(void *)NULL;
		bind[bindcounter].buffer_length=0;
		bind[bindcounter].length=0;
	} else {
		bind[bindcounter].buffer_type=MYSQL_TYPE_STRING;
		bind[bindcounter].buffer=(void *)value;
		bind[bindcounter].buffer_length=valuesize;
		bind[bindcounter].length=&bindvaluesize[bindcounter];
	}
	bind[bindcounter].is_null=(my_bool *)isnull;
	bindcounter++;

	return true;
}

bool mysqlcursor::inputBind(const char *variable, 
				uint16_t variablesize,
				int64_t *value) {

	if (!usestmtprepare) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=sizeof(int64_t);

	bind[bindcounter].buffer_type=MYSQL_TYPE_LONGLONG;
	bind[bindcounter].buffer=(void *)value;
	bind[bindcounter].buffer_length=sizeof(int64_t);
	bind[bindcounter].length=&bindvaluesize[bindcounter];
	bind[bindcounter].is_null=(my_bool *)&(mysqlconn->myfalse);
	bindcounter++;

	return true;
}

bool mysqlcursor::inputBind(const char *variable, 
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {

	if (!usestmtprepare) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=sizeof(double);

	bind[bindcounter].buffer_type=MYSQL_TYPE_DOUBLE;
	bind[bindcounter].buffer=(void *)value;
	bind[bindcounter].buffer_length=sizeof(double);
	bind[bindcounter].length=&bindvaluesize[bindcounter];
	bind[bindcounter].is_null=(my_bool *)&(mysqlconn->myfalse);
	bindcounter++;

	return true;
}

bool mysqlcursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t year,
				int16_t month,
				int16_t day,
				int16_t hour,
				int16_t minute,
				int16_t second,
				int32_t microsecond,
				const char *tz,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {

	if (!usestmtprepare) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=sizeof(MYSQL_TIME);

	if (*isnull) {
		bind[bindcounter].buffer_type=MYSQL_TYPE_NULL;
		bind[bindcounter].buffer=(void *)NULL;
		bind[bindcounter].buffer_length=0;
		bind[bindcounter].length=0;
	} else {
		MYSQL_TIME	*t=(MYSQL_TIME *)buffer;
		t->year=year;
		t->month=month;
		t->day=day;
		t->hour=hour;
		t->minute=minute;
		t->second=second;
		t->second_part=microsecond;
		t->neg=FALSE;
		t->time_type=MYSQL_TIMESTAMP_DATETIME;

		bind[bindcounter].buffer_type=MYSQL_TYPE_DATETIME;
		bind[bindcounter].buffer=(void *)buffer;
		bind[bindcounter].buffer_length=sizeof(MYSQL_TIME);
		bind[bindcounter].length=&bindvaluesize[bindcounter];
	}
	bind[bindcounter].is_null=(my_bool *)isnull;
	bindcounter++;

	return true;
}

bool mysqlcursor::inputBindBlob(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {

	if (!usestmtprepare) {
		return true;
	}

	// don't attempt to bind beyond the number of
	// variables defined when the query was prepared
	if (bindcounter>bindcount) {
		return false;
	}

	bindvaluesize[bindcounter]=valuesize;

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
	bindcounter++;

	return true;
}

bool mysqlcursor::inputBindClob(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {
	return inputBindBlob(variable,variablesize,value,valuesize,isnull);
}
#endif

bool mysqlcursor::executeQuery(const char *query, uint32_t length) {

	// initialize counts
	ncols=0;
	nrows=0;

#ifdef HAVE_MYSQL_STMT_PREPARE
	if (usestmtprepare) {

		// handle binds
		if (bindcounter && mysql_stmt_bind_param(stmt,bind)) {
			return false;
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

		// store the result set
		if (mysql_stmt_store_result(stmt)) {
			return false;
		}

		// get the row count
		nrows=mysql_stmt_num_rows(stmt);

	} else {

#else
		// if this if the first query of the session, do a commit first,
		// doing this will refresh this connection with any data
		// committed by other connections, which is what would happen
		// if a new client connected directly to mysql
		// (if HAVE_MYSQL_STMT_PREPARE is defined,
		// then this is done in prepareQuery())
		if (mysqlconn->firstquery) {
			mysqlconn->commit();
			mysqlconn->firstquery=false;
		}
#endif

		// initialize result set
		mysqlresult=NULL;

		// execute the query
		if ((queryresult=mysql_real_query(&mysqlconn->mysql,
							query,length))) {
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

#ifdef HAVE_MYSQL_STMT_PREPARE
	}
#endif

	// grab the field info
	if (mysqlresult) {
		mysql_field_seek(mysqlresult,0);
		for (unsigned int i=0; i<ncols; i++) {
			mysqlfields[i]=mysql_fetch_field(mysqlresult);
		}
	}

	return true;
}

void mysqlcursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	*liveconnection=true;

	const char	*err;
	unsigned int	errn;
#ifdef HAVE_MYSQL_STMT_PREPARE
	if (usestmtprepare) {
		err=mysql_stmt_error(stmt);
		errn=mysql_stmt_errno(stmt);
	} else {
#endif
		err=mysql_error(&mysqlconn->mysql);
		errn=mysql_errno(&mysqlconn->mysql);
#ifdef HAVE_MYSQL_STMT_PREPARE
	}
#endif

	// Below we check both queryresult and errn.  At one time, we only
	// checked queryresult.  This may have been a bug.  It's possible that
	// back then we should have only checked errn.  But I have a fuzzy
	// memory of some version of mysql returning these error codes in
	// queryresult, so for now I'll leave that code and check both.
#if defined(HAVE_MYSQL_CR_SERVER_GONE_ERROR) || \
		defined(HAVE_MYSQL_CR_SERVER_LOST) 
	#ifdef HAVE_MYSQL_CR_SERVER_GONE_ERROR
		if (queryresult==CR_SERVER_GONE_ERROR ||
				errn==CR_SERVER_GONE_ERROR) {
			*liveconnection=false;
		} else
	#endif
	#ifdef HAVE_MYSQL_CR_SERVER_LOST
		if (queryresult==CR_SERVER_GONE_ERROR /*||
				errn==CR_SERVER_LOST*/) {
			*liveconnection=false;
		} else
	#endif
#endif
	if (!charstring::compare(err,"") ||
		!charstring::compareIgnoringCase(err,
				"mysql server has gone away") ||
		!charstring::compareIgnoringCase(err,
				"Can't connect to local MySQL",28) /*||
		!charstring::compareIgnoringCase(err,
			"Lost connection to MySQL server during query")*/) {
		*liveconnection=false;
	}

	// set return values
	*errorlength=charstring::length(err);
	charstring::safeCopy(errorbuffer,errorbufferlength,err,*errorlength);
	*errorcode=errn;
}

uint32_t mysqlcursor::colCount() {
	return ncols;
}

bool mysqlcursor::knowsRowCount() {
	return true;
}

uint64_t mysqlcursor::rowCount() {
	return nrows;
}

uint64_t mysqlcursor::affectedRows() {
	return affectedrows;
}

const char *mysqlcursor::getColumnName(uint32_t col) {
	return mysqlfields[col]->name;
}

uint16_t mysqlcursor::getColumnType(uint32_t col) {
	if (mysqlfields[col]->type==FIELD_TYPE_STRING) {
		return STRING_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_VAR_STRING) {
		return CHAR_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_DECIMAL
#ifdef HAVE_MYSQL_FIELD_TYPE_NEWDECIMAL
		|| mysqlfields[col]->type==FIELD_TYPE_NEWDECIMAL
#endif
		) {
		return DECIMAL_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_TINY) {
		return TINYINT_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_SHORT) {
		return SMALLINT_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_LONG) {
		return INT_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_FLOAT) {
		return FLOAT_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_DOUBLE) {
		return REAL_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_LONGLONG) {
		return BIGINT_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_INT24) {
		return MEDIUMINT_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_TIMESTAMP) {
		return TIMESTAMP_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_DATE) {
		return DATE_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_TIME) {
		return TIME_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_DATETIME) {
		return DATETIME_DATATYPE;
#ifdef HAVE_MYSQL_FIELD_TYPE_YEAR
	} else if (mysqlfields[col]->type==FIELD_TYPE_YEAR) {
		return YEAR_DATATYPE;
#endif
#ifdef HAVE_MYSQL_FIELD_TYPE_NEWDATE
	} else if (mysqlfields[col]->type==FIELD_TYPE_NEWDATE) {
		return NEWDATE_DATATYPE;
#endif
	} else if (mysqlfields[col]->type==FIELD_TYPE_NULL) {
		return NULL_DATATYPE;
#ifdef HAVE_MYSQL_FIELD_TYPE_ENUM
	} else if (mysqlfields[col]->type==FIELD_TYPE_ENUM) {
		return ENUM_DATATYPE;
#endif
#ifdef HAVE_MYSQL_FIELD_TYPE_SET
	} else if (mysqlfields[col]->type==FIELD_TYPE_SET) {
		return SET_DATATYPE;
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
	} else if (mysqlfields[col]->type==FIELD_TYPE_TINY_BLOB ||
			(mysqlfields[col]->type==FIELD_TYPE_BLOB &&
					mysqlfields[col]->length<256)) {
		return TINY_BLOB_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_BLOB &&
					mysqlfields[col]->length<65536) {
		return BLOB_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_MEDIUM_BLOB ||
			(mysqlfields[col]->type==FIELD_TYPE_BLOB &&
					mysqlfields[col]->length<16777216)) {
		return MEDIUM_BLOB_DATATYPE;
	} else if (mysqlfields[col]->type==FIELD_TYPE_LONG_BLOB ||
				mysqlfields[col]->type==FIELD_TYPE_BLOB) {
		return LONG_BLOB_DATATYPE;
	} else {
		return UNKNOWN_DATATYPE;
	}
}

uint32_t mysqlcursor::getColumnLength(uint32_t col) {

	if (getColumnType(col)==STRING_DATATYPE) {
		return (uint32_t)mysqlfields[col]->length;
	} else if (getColumnType(col)==CHAR_DATATYPE) {
		return (uint32_t)mysqlfields[col]->length+1;
	} else if (getColumnType(col)==DECIMAL_DATATYPE) {
		uint32_t	length=0;
		if (mysqlfields[col]->decimals>0) {
			length=mysqlfields[col]->length+2;
		} else if (mysqlfields[col]->decimals==0) {
			length=mysqlfields[col]->length+1;
		}
		if (mysqlfields[col]->length<mysqlfields[col]->decimals) {
			length=mysqlfields[col]->decimals+2;
		}
		return length;
	} else if (getColumnType(col)==TINYINT_DATATYPE) {
		return 1;
	} else if (getColumnType(col)==SMALLINT_DATATYPE) {
		return 2;
	} else if (getColumnType(col)==INT_DATATYPE) {
		return 4;
	} else if (getColumnType(col)==FLOAT_DATATYPE) {
		if (mysqlfields[col]->length<=24) {
			return 4;
		} else {
			return 8;
		}
	} else if (getColumnType(col)==REAL_DATATYPE) {
		return 8;
	} else if (getColumnType(col)==BIGINT_DATATYPE) {
		return 8;
	} else if (getColumnType(col)==MEDIUMINT_DATATYPE) {
		return 3;
	} else if (getColumnType(col)==TIMESTAMP_DATATYPE) {
		return 4;
	} else if (getColumnType(col)==DATE_DATATYPE) {
		return 3;
	} else if (getColumnType(col)==TIME_DATATYPE) {
		return 3;
	} else if (getColumnType(col)==DATETIME_DATATYPE) {
		return 8;
#ifdef HAVE_MYSQL_FIELD_TYPE_YEAR
	} else if (getColumnType(col)==YEAR_DATATYPE) {
		return 1;
#endif
#ifdef HAVE_MYSQL_FIELD_TYPE_NEWDATE
	} else if (getColumnType(col)==NEWDATE_DATATYPE) {
		return 1;
#endif
	} else if (getColumnType(col)==NULL_DATATYPE) {
#ifdef HAVE_MYSQL_FIELD_TYPE_ENUM
	} else if (getColumnType(col)==ENUM_DATATYPE) {
		// 1 or 2 bytes delepending on the # of enum values (65535 max)
		return 2;
#endif
#ifdef HAVE_MYSQL_FIELD_TYPE_SET
	} else if (getColumnType(col)==SET_DATATYPE) {
		// 1,2,3,4 or 8 bytes depending on the # of members (64 max)
		return 8;
#endif
	} else if (getColumnType(col)==TINY_BLOB_DATATYPE) {
		return 255;
	} else if (getColumnType(col)==BLOB_DATATYPE) {
		return 65535;
	} else if (getColumnType(col)==MEDIUM_BLOB_DATATYPE) {
		return 16777215;
	} else if (getColumnType(col)==LONG_BLOB_DATATYPE) {
		return 2147483647;
	}
	return (uint32_t)mysqlfields[col]->length;
}

uint32_t mysqlcursor::getColumnPrecision(uint32_t col) {
	return mysqlfields[col]->length;
}

uint32_t mysqlcursor::getColumnScale(uint32_t col) {
	return mysqlfields[col]->decimals;
}

uint16_t mysqlcursor::getColumnIsNullable(uint32_t col) {
	return !(IS_NOT_NULL(mysqlfields[col]->flags));
}

uint16_t mysqlcursor::getColumnIsPrimaryKey(uint32_t col) {
	return IS_PRI_KEY(mysqlfields[col]->flags);
}

uint16_t mysqlcursor::getColumnIsUnique(uint32_t col) {
	return mysqlfields[col]->flags&UNIQUE_KEY_FLAG;
}

uint16_t mysqlcursor::getColumnIsPartOfKey(uint32_t col) {
	return mysqlfields[col]->flags&MULTIPLE_KEY_FLAG;
}

uint16_t mysqlcursor::getColumnIsUnsigned(uint32_t col) {
	return mysqlfields[col]->flags&UNSIGNED_FLAG;
}

uint16_t mysqlcursor::getColumnIsZeroFilled(uint32_t col) {
	return mysqlfields[col]->flags&ZEROFILL_FLAG;
}

uint16_t mysqlcursor::getColumnIsBinary(uint32_t col) {
	#ifdef BINARY_FLAG
		return mysqlfields[col]->flags&BINARY_FLAG;
	#else
		return 0;
	#endif
}

uint16_t mysqlcursor::getColumnIsAutoIncrement(uint32_t col) {
	#ifdef AUTO_INCREMENT_FLAG
		return mysqlfields[col]->flags&AUTO_INCREMENT_FLAG;
	#else
		return 0;
	#endif
}

bool mysqlcursor::noRowsToReturn() {
	// for DML or DDL queries, return no data
	return (!mysqlresult);
}

bool mysqlcursor::fetchRow() {
#ifdef HAVE_MYSQL_STMT_PREPARE
	if (usestmtprepare) {
		return !mysql_stmt_fetch(stmt);
	} else {
#endif
		return ((mysqlrow=mysql_fetch_row(mysqlresult))!=NULL &&
			(mysqlrowlengths=mysql_fetch_lengths(
						mysqlresult))!=NULL);
#ifdef HAVE_MYSQL_STMT_PREPARE
	}
#endif
}

void mysqlcursor::getField(uint32_t col,
				const char **fld, uint64_t *fldlength,
				bool *blob, bool *null) {

#ifdef HAVE_MYSQL_STMT_PREPARE
	if (usestmtprepare) {
		if (!isnull[col]) {
			*fld=field[col];
			*fldlength=fieldlength[col];
		} else {
			*null=true;
		}
	} else {
#endif
		if (mysqlrow[col]) {
			*fld=mysqlrow[col];
			*fldlength=mysqlrowlengths[col];
		} else {
			*null=true;
		}
#ifdef HAVE_MYSQL_STMT_PREPARE
	}
#endif
}

void mysqlcursor::cleanUpData() {
#ifdef HAVE_MYSQL_STMT_PREPARE
	if (usestmtprepare) {
		bindcounter=0;
		for (uint16_t i=0; i<conn->cont->maxbindcount; i++) {
			rawbuffer::zero(&bind[i],sizeof(MYSQL_BIND));
		}
		mysql_stmt_reset(stmt);
		if (stmtfreeresult) {
			mysql_stmt_free_result(stmt);
			stmtfreeresult=false;
		}
	}
#endif
	if (mysqlresult!=(MYSQL_RES *)NULL) {
		mysql_free_result(mysqlresult);
		mysqlresult=NULL;
#ifdef HAVE_MYSQL_NEXT_RESULT
		while (!mysql_next_result(&mysqlconn->mysql)) {
			mysqlresult=mysql_store_result(&mysqlconn->mysql);
			if (mysqlresult!=(MYSQL_RES *)NULL) {
				mysql_free_result(mysqlresult);
				mysqlresult=NULL;
			}
		}
#endif
	}
}
