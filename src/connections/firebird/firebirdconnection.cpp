// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrconnection.h>
#include <rudiments/environment.h>
#include <rudiments/bytestring.h>
#include <rudiments/snooze.h>
#include <rudiments/sys.h>

#include <defines.h>
#include <datatypes.h>
#include <config.h>

#include <ibase.h>

// for pow()
#include <math.h>

// for struct tm
#include <time.h>

#define MAX_ITEM_BUFFER_SIZE 4096
#define MAX_SELECT_LIST_SIZE 256
#define MAX_BIND_VARS 512
#define MAX_LOB_CHUNK_SIZE 65535

struct fieldstruct {
	int		sqlrtype;
	short		type;

	short		shortbuffer;
	long		longbuffer;
	float		floatbuffer;
	double		doublebuffer;
	ISC_QUAD	quadbuffer;
	ISC_DATE	datebuffer;
	ISC_TIME	timebuffer;
	ISC_TIMESTAMP	timestampbuffer;
	ISC_INT64	int64buffer;
	char		textbuffer[MAX_ITEM_BUFFER_SIZE+1];
	ISC_QUAD	blobid;
	isc_blob_handle	blobhandle;
	bool		blobisopen;

	short		nullindicator;
};

struct datebind {
        int16_t         *year;
        int16_t         *month;
        int16_t         *day;
        int16_t         *hour;
        int16_t         *minute;
        int16_t         *second;
        const char      **tz;
};

class firebirdconnection;

class firebirdcursor : public sqlrcursor_svr {
	friend class firebirdconnection;
	private:
				firebirdcursor(sqlrconnection_svr *conn);
				~firebirdcursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						short *isnull);
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
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value, 
						uint16_t valuesize,
						short *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBindBlob(const char *variable,
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindClob(const char *variable,
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		getLobOutputBindLength(uint16_t index,
							uint64_t *length);
		bool		getLobOutputBindSegment(uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		void		cleanUpLobOutputBind(uint16_t index);
		bool		executeQuery(const char *query,
						uint32_t length);
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		bool		queryIsNotSelect();
		bool		queryIsCommitOrRollback();
		bool		knowsAffectedRows();
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnNameLength(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		bool		getLobFieldLength(uint32_t col,
					uint64_t *length);
		bool		getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread);
		void		cleanUpLobField(uint32_t col);
		void		cleanUpData();


		isc_stmt_handle	stmt;
		uint16_t	outbindcount;
		datebind	outdatebind[MAX_BIND_VARS];
		
		XSQLDA	ISC_FAR	*outsqlda;
		ISC_QUAD	*outblobid;
		isc_blob_handle	*outblobhandle;
		bool		*outblobisopen;

		XSQLDA	ISC_FAR	*insqlda;
		ISC_QUAD	*inblobid;
		isc_blob_handle	*inblobhandle;

		ISC_LONG	querytype;

		fieldstruct	field[MAX_SELECT_LIST_SIZE];
		stringbuffer	fieldbuffer;

		firebirdconnection	*firebirdconn;

		bool	queryisexecsp;
		bool	bindformaterror;
};

class firebirdconnection : public sqlrconnection_svr {
	friend class firebirdcursor;
	public:
			firebirdconnection(sqlrcontroller_svr *cont);
			~firebirdconnection();
	private:
		void	handleConnectString();
		bool	logIn(const char **error);
		sqlrcursor_svr	*initCursor();
		void	deleteCursor(sqlrcursor_svr *curs);
		void	logOut();
		bool	supportsTransactionBlocks();
		bool	commit();
		bool	rollback();
		bool	ping();
		void	errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t	*errorcode,
					bool *liveconnection);
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*getDatabaseListQuery(bool wild);
		const char	*getTableListQuery(bool wild);
		const char	*getColumnListQuery(const char *table,
								bool wild);
		const char	*bindFormat();
		const char	*getLastInsertIdQuery();

		char		dpb[256];
		short		dpblength;
		isc_db_handle	db;
		isc_tr_handle	tr;

		const char	*database;
		char		*host;
		unsigned short	dialect;

		const char	*charset;

		char		*dbversion;

		char		*lastinsertidquery;

		ISC_STATUS	error[20];

		stringbuffer	errormsg;
};

static char tpb[] = {
	isc_tpb_version3,
	isc_tpb_write,
	isc_tpb_read_committed,
	isc_tpb_rec_version,
	// FIXME: vladimir changed this to isc_tpb_nowait.  why?
	isc_tpb_wait
};

firebirdconnection::firebirdconnection(sqlrcontroller_svr *cont) :
						sqlrconnection_svr(cont) {
	dbversion=NULL;
	lastinsertidquery=NULL;
	host=NULL;
}

firebirdconnection::~firebirdconnection() {
	delete dbversion;
	delete[] lastinsertidquery;
	delete[] host;
}

void firebirdconnection::handleConnectString() {

	// override legacy "database" parameter with modern "db" parameter
	database=cont->connectStringValue("database");
	const char	*tmp=cont->connectStringValue("db");
	if (tmp && tmp[0]) {
		database=tmp;
	}

	// parse the host name from the database
	const char	*colon=charstring::findFirst(database,':');
	if (colon) {
		host=charstring::duplicate(database,colon-database);
	} else {
		host=sys::getHostName();
	}

	const char	*dialectstr=cont->connectStringValue("dialect");
	if (dialectstr) {
		dialect=charstring::toInteger(dialectstr);
		if (dialect<1) {
			dialect=1;
		}
		if (dialect>3) {
			dialect=3;
		}
	} else {
		dialect=3;
	}
	cont->setUser(cont->connectStringValue("user"));
	cont->setPassword(cont->connectStringValue("password"));
	const char	*autocom=cont->connectStringValue("autocommit");
	cont->setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));

	charset=cont->connectStringValue("charset");

	cont->setFakeTransactionBlocksBehavior(
		!charstring::compare(
			cont->connectStringValue("faketransactionblocks"),
			"yes"));

	const char	*lastinsertidfunc=
			cont->connectStringValue("lastinsertidfunction");
	if (lastinsertidfunc) {
		stringbuffer	liiquery;
		liiquery.append("select id from ");
		liiquery.append(lastinsertidfunc);
		lastinsertidquery=liiquery.detachString();
	}

	cont->fakeinputbinds=
		!charstring::compare(
			cont->connectStringValue("fakebinds"),"yes");
}

bool firebirdconnection::logIn(const char **err) {

	// initialize a parameter buffer
	char	*dpbptr=dpb;

	// set the parameter buffer version
	*dpbptr=isc_dpb_version1;
	dpbptr++;

	// no idea what this does, something involving the "cache"
	*dpbptr=isc_dpb_num_buffers;
	dpbptr++;
	*dpbptr=1;
	dpbptr++;
	*dpbptr=90;
	dpbptr++;

	// set the character set
	if (charstring::length(charset)) {
		*dpbptr=isc_dpb_lc_ctype;
		dpbptr++;
		*dpbptr=charstring::length(charset);
		dpbptr++;
		charstring::copy(dpbptr,charset);
		dpbptr+=charstring::length(charset);
	}

	// determine the parameter buffer length
	dpblength=dpbptr-dpb;

	// handle user/password parameters
	const char	*user=cont->getUser();
	if (user) {
		environment::setValue("ISC_USER",user);
	}
	const char	*password=cont->getPassword();
	if (password) {
		environment::setValue("ISC_PASSWORD",password);
	}

	// attach to the database
	db=0L;
	tr=0L;
	if (isc_attach_database(error,charstring::length(database),
					const_cast<char *>(database),&db,
					dpblength,dpb)) {
		db=0L;
		return false;
	}

	// start a transaction
	if (isc_start_transaction(error,&tr,1,&db,(uint16_t)sizeof(tpb),&tpb)) {

		errormsg.clear();

		char		msg[512];
		ISC_STATUS	*errstatus=error;
		bool		first=false;
		while (isc_interprete(msg,&errstatus)) {
			if (first) {
				errormsg.append(": ");
			}
			errormsg.append(msg);
			first=true;
		}
		*err=errormsg.getString();
		return false;
	}

	return true;
}

sqlrcursor_svr *firebirdconnection::initCursor() {
	return (sqlrcursor_svr *)new firebirdcursor(
					(sqlrconnection_svr *)this);
}

void firebirdconnection::deleteCursor(sqlrcursor_svr *curs) {
	delete (firebirdcursor *)curs;
}

void firebirdconnection::logOut() {
	isc_detach_database(error,&db);
}

bool firebirdconnection::supportsTransactionBlocks() {
	return false;
}

bool firebirdconnection::commit() {
	return (!isc_commit_retaining(error,&tr));
}

bool firebirdconnection::rollback() {
	return (!isc_rollback_retaining(error,&tr));
}

void firebirdconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	// declare a buffer for the error
	errormsg.clear();

	char		msg[512];
	ISC_STATUS	*pvector=error;

	// get the status message
	while (isc_interprete(msg,&pvector)) {
		errormsg.append(msg)->append(" \n");
	}

	// get the error message
	ISC_LONG	sqlcode=isc_sqlcode(error);
	isc_sql_interprete(sqlcode,errorbuffer,errorbufferlength);

	// set return values
	*errorlength=charstring::length(errorbuffer);
	*errorcode=sqlcode;
	*liveconnection=!(charstring::contains(
				errormsg.getString(),
				"Error reading data from the connection") ||
			charstring::contains(
				errormsg.getString(),
				"Error writing data to the connection"));
}

bool firebirdconnection::ping() {

	// call isc_database_info to get page_size and num_buffers,
	// this should always be available unless the db is down
	// if we get an error, then return 0, otherwise return 1
	ISC_STATUS	status[20];
	char		dbitems[]={isc_info_page_size,
					isc_info_num_buffers,
					isc_info_end};
	char		resbuffer[40];

	isc_database_info(status,&db,
				sizeof(dbitems),dbitems,
				sizeof(resbuffer),resbuffer);

	return !(status[0]==1 && status[1]);
}

const char *firebirdconnection::identify() {
	return "firebird";
}

const char *firebirdconnection::dbVersion() {
	ISC_STATUS	status[20];
	char		dbitems[]={isc_info_version,
					isc_info_end};
	char		resbuffer[256];
	if (!isc_database_info(status,&db,
				sizeof(dbitems),dbitems,
				sizeof(resbuffer),resbuffer)) {

		char	*ptr=resbuffer;

		// first byte is isc_info_version
		ptr++;

		// next 2 bytes are length of the isc_info_version data
		ptr=ptr+sizeof(uint16_t);

		// the next byte is the number of lines of text
		stringbuffer	dbvers;
		char	linecount=*ptr;
		ptr++;
		for (char lineindex=0; lineindex<linecount; lineindex++) {

			// the first byte of each line is the length of the line
			char	linelen=*ptr;
			ptr++;

			// then comes the line of text itself
			if (lineindex) {
				dbvers.append('\n');
			}
			dbvers.append(ptr,linelen);
		}

		delete[] dbversion;
		dbversion=dbvers.detachString();
		return dbversion;
	} 
	return "";
}

const char *firebirdconnection::dbHostName() {
	return host;
}

const char *firebirdconnection::getDatabaseListQuery(bool wild) {
	return "select '' from rdb$database";
}

const char *firebirdconnection::getTableListQuery(bool wild) {
	return (wild)?
		"select "
		"	rdb$relation_name "
		"from "
		"	rdb$relations "
		"where "
		"	rdb$system_flag=0 "
		"	and "
		"	rdb$relation_name like '%s' "
		"order by "
		"	rdb$relation_name":

		"select "
		"	rdb$relation_name "
		"from "
		"	rdb$relations "
		"where "
		"	rdb$system_flag=0 "
		"order by "
		"	rdb$relation_name";
}

const char *firebirdconnection::getColumnListQuery(const char *table,
								bool wild) {
	return (wild)?
		"select "
		"	r.rdb$field_name, "
		"	case f.rdb$field_type "
		"		when 261 then 'BLOB' "
		"		when 14 then 'CHAR' "
		"		when 40 then 'CSTRING' "
		"		when 11 then 'D_FLOAT' "
		"		when 27 then 'DOUBLE' "
		"		when 10 then 'FLOAT' "
		"		when 16 then 'INT64' "
		"		when 8 then 'INTEGER' "
		"		when 9 then 'QUAD' "
		"		when 7 then 'SMALLINT' "
		"		when 12 then 'DATE' "
		"		when 13 then 'TIME' "
		"		when 35 then 'TIMESTAMP' "
		"		when 37 then 'VARCHAR' "
		"		else 'UNKNOWN' "
		"		end as field_type, "
		"	f.rdb$field_length, "
		"	f.rdb$field_precision, "
		"	f.rdb$field_scale, "
		"	r.rdb$null_flag, "
		"	'' as primary_key, "
		"	'' as default_value, "
		"	'' as extra "
		"from "
		"	rdb$relation_fields r "
		"	left join "
		"		rdb$fields f "
		"		on "
		"		f.rdb$field_name=r.rdb$field_source "
		"where "
		"	r.rdb$relation_name=upper('%s') "
		"	and "
		"	r.rdb$field_name like '%s' "
		"order by "
		"	rdb$field_position":

		"select "
		"	r.rdb$field_name, "
		"	case f.rdb$field_type "
		"		when 261 then 'BLOB' "
		"		when 14 then 'CHAR' "
		"		when 40 then 'CSTRING' "
		"		when 11 then 'D_FLOAT' "
		"		when 27 then 'DOUBLE' "
		"		when 10 then 'FLOAT' "
		"		when 16 then 'INT64' "
		"		when 8 then 'INTEGER' "
		"		when 9 then 'QUAD' "
		"		when 7 then 'SMALLINT' "
		"		when 12 then 'DATE' "
		"		when 13 then 'TIME' "
		"		when 35 then 'TIMESTAMP' "
		"		when 37 then 'VARCHAR' "
		"		else 'UNKNOWN' "
		"		end as field_type, "
		"	f.rdb$field_length, "
		"	f.rdb$field_precision, "
		"	f.rdb$field_scale, "
		"	r.rdb$null_flag, "
		"	'' as primary_key, "
		"	'' as default_value, "
		"	'' as extra "
		"from "
		"	rdb$relation_fields r "
		"	left join "
		"		rdb$fields f "
		"		on "
		"		f.rdb$field_name=r.rdb$field_source "
		"where "
		"	r.rdb$relation_name=upper('%s') "
		"order by "
		"	r.rdb$field_position";
}

const char *firebirdconnection::bindFormat() {
	return "?";
}

const char *firebirdconnection::getLastInsertIdQuery() {
	return lastinsertidquery;
}

firebirdcursor::firebirdcursor(sqlrconnection_svr *conn) :
						sqlrcursor_svr(conn) {
	firebirdconn=(firebirdconnection *)conn;

	outsqlda=(XSQLDA ISC_FAR *)new unsigned char[
					XSQLDA_LENGTH(MAX_SELECT_LIST_SIZE)];
	outsqlda->version=SQLDA_VERSION1;
	outsqlda->sqln=(MAX_SELECT_LIST_SIZE>MAX_BIND_VARS)?
				MAX_SELECT_LIST_SIZE:MAX_BIND_VARS;
	outblobid=new ISC_QUAD[MAX_BIND_VARS];
	outblobhandle=new isc_blob_handle[MAX_BIND_VARS];
	outblobisopen=new bool[MAX_BIND_VARS];

	insqlda=(XSQLDA ISC_FAR *)new unsigned char[
					XSQLDA_LENGTH(MAX_BIND_VARS)];
	insqlda->version=SQLDA_VERSION1;
	insqlda->sqln=MAX_BIND_VARS;
	inblobid=new ISC_QUAD[MAX_BIND_VARS];
	inblobhandle=new isc_blob_handle[MAX_BIND_VARS];

	querytype=0;
	stmt=NULL;

	queryisexecsp=false;
	bindformaterror=false;

	outbindcount=0;
}

firebirdcursor::~firebirdcursor() {
	delete[] outsqlda;
	delete[] outblobid;
	delete[] outblobhandle;
	delete[] outblobisopen;

	delete[] insqlda;
	delete[] inblobid;
	delete[] inblobhandle;
}

bool firebirdcursor::prepareQuery(const char *query, uint32_t length) {

	queryisexecsp=false;

	// reset the bind format error flag
	bindformaterror=false;

	// free the old statement if it exists
	if (stmt) {
		isc_dsql_free_statement(firebirdconn->error,
						&stmt,DSQL_drop);
	}

	// allocate a cursor handle
	stmt=NULL;
	if (isc_dsql_allocate_statement(firebirdconn->error,
					&firebirdconn->db,&stmt)) {
		return false;
	}

	// skip whitespace
	char	*qptr=(char *)query;
	while (*qptr==' ' || *qptr=='\n' || *qptr=='	');

	// prepare the cursor
	if (isc_dsql_prepare(firebirdconn->error,&firebirdconn->tr,
					&stmt,length,(char *)query,
					firebirdconn->dialect,outsqlda)) {
		return false;
	}

	// get the cursor type
	char	typeitem[]={isc_info_sql_stmt_type};
	char	resbuffer[1024];
	if (isc_dsql_sql_info(firebirdconn->error,&stmt,
				sizeof(typeitem),typeitem,
				1024,resbuffer)) {
		return false;
	}

	// (modern versions of isc_vax_integer take a const char * parameter,
	// but old versions take char * and this cast works with both)
	ISC_LONG	len=isc_vax_integer((char *)(resbuffer+1),2);
	querytype=isc_vax_integer((char *)(resbuffer+3),len);

	// find bind parameters, if any
	insqlda->sqld=0;
	if (isc_dsql_describe_bind(firebirdconn->error,&stmt,1,insqlda)) {
		return false;
	}
	insqlda->sqln=insqlda->sqld;

	return true;
}

bool firebirdcursor::inputBind(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}
	insqlda->sqlvar[index].sqltype=SQL_TEXT+1;
	insqlda->sqlvar[index].sqlscale=0;
	insqlda->sqlvar[index].sqlsubtype=0;
	insqlda->sqlvar[index].sqllen=valuesize;
	insqlda->sqlvar[index].sqldata=(char *)value;
	insqlda->sqlvar[index].sqlind=isnull;
	insqlda->sqlvar[index].sqlname_length=0;
	insqlda->sqlvar[index].sqlname[0]='\0';
	insqlda->sqlvar[index].relname_length=0;
	insqlda->sqlvar[index].relname[0]='\0';
	insqlda->sqlvar[index].ownname_length=0;
	insqlda->sqlvar[index].ownname[0]='\0';
	insqlda->sqlvar[index].aliasname_length=0;
	insqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::inputBind(const char *variable,
					uint16_t variablesize,
					int64_t *value) {

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}
	insqlda->sqlvar[index].sqltype=SQL_INT64;
	insqlda->sqlvar[index].sqlscale=0;
	insqlda->sqlvar[index].sqlsubtype=0;
	insqlda->sqlvar[index].sqllen=sizeof(int64_t);
	insqlda->sqlvar[index].sqldata=(char *)value;
	insqlda->sqlvar[index].sqlind=(short *)NULL;
	insqlda->sqlvar[index].sqlname_length=0;
	insqlda->sqlvar[index].sqlname[0]='\0';
	insqlda->sqlvar[index].relname_length=0;
	insqlda->sqlvar[index].relname[0]='\0';
	insqlda->sqlvar[index].ownname_length=0;
	insqlda->sqlvar[index].ownname[0]='\0';
	insqlda->sqlvar[index].aliasname_length=0;
	insqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::inputBind(const char *variable,
					uint16_t variablesize,
					double *value,
					uint32_t precision,
					uint32_t scale) {

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}
	insqlda->sqlvar[index].sqltype=SQL_DOUBLE;
	insqlda->sqlvar[index].sqlscale=scale;
	insqlda->sqlvar[index].sqlsubtype=0;
	insqlda->sqlvar[index].sqllen=sizeof(double);
	insqlda->sqlvar[index].sqldata=(char *)value;
	insqlda->sqlvar[index].sqlind=(short *)NULL;
	insqlda->sqlvar[index].sqlname_length=0;
	insqlda->sqlvar[index].sqlname[0]='\0';
	insqlda->sqlvar[index].relname_length=0;
	insqlda->sqlvar[index].relname[0]='\0';
	insqlda->sqlvar[index].ownname_length=0;
	insqlda->sqlvar[index].ownname[0]='\0';
	insqlda->sqlvar[index].aliasname_length=0;
	insqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::inputBind(const char *variable,
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

	// build an ISC_TIMESTAMP
	tm	t;
	t.tm_sec=second;
	t.tm_min=minute;
	t.tm_hour=hour;
	t.tm_mday=day;
	t.tm_mon=month-1;
	t.tm_year=year-1900;
	isc_encode_timestamp(&t,(ISC_TIMESTAMP *)buffer);

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}
	insqlda->sqlvar[index].sqltype=SQL_TIMESTAMP;
	insqlda->sqlvar[index].sqlscale=0;
	insqlda->sqlvar[index].sqlsubtype=0;
	insqlda->sqlvar[index].sqllen=sizeof(ISC_TIMESTAMP);
	insqlda->sqlvar[index].sqldata=buffer;
	insqlda->sqlvar[index].sqlind=(short *)NULL;
	insqlda->sqlvar[index].sqlname_length=0;
	insqlda->sqlvar[index].sqlname[0]='\0';
	insqlda->sqlvar[index].relname_length=0;
	insqlda->sqlvar[index].relname[0]='\0';
	insqlda->sqlvar[index].ownname_length=0;
	insqlda->sqlvar[index].ownname[0]='\0';
	insqlda->sqlvar[index].aliasname_length=0;
	insqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}

	// create a blob
	if (isc_create_blob2(firebirdconn->error,
				&firebirdconn->db,
				&firebirdconn->tr,
				&inblobhandle[index],
				&inblobid[index],0,NULL)) {
		return false;
	}

	// write the value to the blob, MAX_LOB_CHUNK_SIZE bytes at a time
	uint16_t	bytesput=0;
	while (bytesput<valuesize) {
		uint16_t	bytestoput=0;
		if (valuesize-bytesput<MAX_LOB_CHUNK_SIZE) {
			bytestoput=valuesize-bytesput;
		} else {
			bytestoput=MAX_LOB_CHUNK_SIZE;
		}
		// (modern versions of isc_put_segment take a const char *
		// parameter, but old versions take char * and this cast works
		// with both)
		if (isc_put_segment(firebirdconn->error,
					&inblobhandle[index],
					bytestoput,(char *)(value+bytesput))) {
			return false;
		}
		bytesput=bytesput+bytestoput;
	}

	// close the blob
	isc_close_blob(firebirdconn->error,&inblobhandle[index]);

	insqlda->sqlvar[index].sqltype=SQL_BLOB+1;
	insqlda->sqlvar[index].sqlscale=0;
	insqlda->sqlvar[index].sqlsubtype=0;
	insqlda->sqlvar[index].sqllen=sizeof(ISC_QUAD);
	insqlda->sqlvar[index].sqldata=(char *)&inblobid[index];
	insqlda->sqlvar[index].sqlind=isnull;
	insqlda->sqlvar[index].sqlname_length=0;
	insqlda->sqlvar[index].sqlname[0]='\0';
	insqlda->sqlvar[index].relname_length=0;
	insqlda->sqlvar[index].relname[0]='\0';
	insqlda->sqlvar[index].ownname_length=0;
	insqlda->sqlvar[index].ownname[0]='\0';
	insqlda->sqlvar[index].aliasname_length=0;
	insqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::inputBindClob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	return inputBindBlob(variable,variablesize,
				value,valuesize,isnull);
}

bool firebirdcursor::outputBind(const char *variable, 
				uint16_t variablesize,
				char *value, 
				uint16_t valuesize, 
				int16_t *isnull) {

	outbindcount++;

	// if we're doing output binds then the
	// query must be a stored procedure
	queryisexecsp=true;

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}
	outsqlda->sqlvar[index].sqltype=SQL_TEXT+1;
	outsqlda->sqlvar[index].sqlscale=0;
	outsqlda->sqlvar[index].sqlsubtype=0;
	outsqlda->sqlvar[index].sqllen=valuesize;
	outsqlda->sqlvar[index].sqldata=value;
	outsqlda->sqlvar[index].sqlind=isnull;
	outsqlda->sqlvar[index].sqlname_length=0;
	outsqlda->sqlvar[index].sqlname[0]='\0';
	outsqlda->sqlvar[index].relname_length=0;
	outsqlda->sqlvar[index].relname[0]='\0';
	outsqlda->sqlvar[index].ownname_length=0;
	outsqlda->sqlvar[index].ownname[0]='\0';
	outsqlda->sqlvar[index].aliasname_length=0;
	outsqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::outputBind(const char *variable,
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {

	outbindcount++;

	// if we're doing output binds then the
	// query must be a stored procedure
	queryisexecsp=true;

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}
	outsqlda->sqlvar[index].sqltype=SQL_INT64;
	outsqlda->sqlvar[index].sqlscale=0;
	outsqlda->sqlvar[index].sqlsubtype=0;
	outsqlda->sqlvar[index].sqllen=sizeof(int64_t);
	outsqlda->sqlvar[index].sqldata=(char *)value;
	outsqlda->sqlvar[index].sqlind=isnull;
	outsqlda->sqlvar[index].sqlname_length=0;
	outsqlda->sqlvar[index].sqlname[0]='\0';
	outsqlda->sqlvar[index].relname_length=0;
	outsqlda->sqlvar[index].relname[0]='\0';
	outsqlda->sqlvar[index].ownname_length=0;
	outsqlda->sqlvar[index].ownname[0]='\0';
	outsqlda->sqlvar[index].aliasname_length=0;
	outsqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::outputBind(const char *variable,
				uint16_t variablesize,
				double *value,
				uint32_t *precision,
				uint32_t *scale,
				int16_t *isnull) {

	outbindcount++;

	// if we're doing output binds then the
	// query must be a stored procedure
	queryisexecsp=true;

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}
	outsqlda->sqlvar[index].sqltype=SQL_DOUBLE;
	outsqlda->sqlvar[index].sqlscale=*scale;
	outsqlda->sqlvar[index].sqlsubtype=0;
	outsqlda->sqlvar[index].sqllen=sizeof(double);
	outsqlda->sqlvar[index].sqldata=(char *)value;
	outsqlda->sqlvar[index].sqlind=isnull;
	outsqlda->sqlvar[index].sqlname_length=0;
	outsqlda->sqlvar[index].sqlname[0]='\0';
	outsqlda->sqlvar[index].relname_length=0;
	outsqlda->sqlvar[index].relname[0]='\0';
	outsqlda->sqlvar[index].ownname_length=0;
	outsqlda->sqlvar[index].ownname[0]='\0';
	outsqlda->sqlvar[index].aliasname_length=0;
	outsqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::outputBind(const char *variable,
				uint16_t variablesize,
				int16_t *year,
				int16_t *month,
				int16_t *day,
				int16_t *hour,
				int16_t *minute,
				int16_t *second,
				int32_t *microsecond,
				const char **tz,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {

	// store the pointers
	outdatebind[outbindcount].year=year;
	outdatebind[outbindcount].month=month;
	outdatebind[outbindcount].day=day;
	outdatebind[outbindcount].hour=hour;
	outdatebind[outbindcount].minute=minute;
	outdatebind[outbindcount].second=second;
	outdatebind[outbindcount].tz=tz;

	outbindcount++;

	// if we're doing output binds then the
	// query must be a stored procedure
	queryisexecsp=true;

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}
	outsqlda->sqlvar[index].sqltype=SQL_TIMESTAMP;
	outsqlda->sqlvar[index].sqlscale=0;
	outsqlda->sqlvar[index].sqlsubtype=0;
	outsqlda->sqlvar[index].sqllen=sizeof(ISC_TIMESTAMP);
	outsqlda->sqlvar[index].sqldata=buffer;
	outsqlda->sqlvar[index].sqlind=isnull;
	outsqlda->sqlvar[index].sqlname_length=0;
	outsqlda->sqlvar[index].sqlname[0]='\0';
	outsqlda->sqlvar[index].relname_length=0;
	outsqlda->sqlvar[index].relname[0]='\0';
	outsqlda->sqlvar[index].ownname_length=0;
	outsqlda->sqlvar[index].ownname[0]='\0';
	outsqlda->sqlvar[index].aliasname_length=0;
	outsqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::outputBindBlob(const char *variable,
					uint16_t variablesize,
					uint16_t ind,
					int16_t *isnull) {

	outbindcount++;

	// if we're doing output binds then the
	// query must be a stored procedure
	queryisexecsp=true;

	// make bind vars 1 based like all other db's
	long	index=charstring::toInteger(variable+1)-1;
	if (index<0) {
		bindformaterror=true;
		return false;
	}

	outblobisopen[index]=false;

	outsqlda->sqlvar[index].sqltype=SQL_BLOB+1;
	outsqlda->sqlvar[index].sqlscale=0;
	outsqlda->sqlvar[index].sqlsubtype=0;
	outsqlda->sqlvar[index].sqllen=sizeof(ISC_QUAD);
	outsqlda->sqlvar[index].sqldata=(char *)&outblobid[index];
	outsqlda->sqlvar[index].sqlind=isnull;
	outsqlda->sqlvar[index].sqlname_length=0;
	outsqlda->sqlvar[index].sqlname[0]='\0';
	outsqlda->sqlvar[index].relname_length=0;
	outsqlda->sqlvar[index].relname[0]='\0';
	outsqlda->sqlvar[index].ownname_length=0;
	outsqlda->sqlvar[index].ownname[0]='\0';
	outsqlda->sqlvar[index].aliasname_length=0;
	outsqlda->sqlvar[index].aliasname[0]='\0';
	return true;
}

bool firebirdcursor::outputBindClob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	return outputBindBlob(variable,variablesize,index,isnull);
}

bool firebirdcursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {

	// open the blob
	outblobhandle[index]=NULL;
	if (isc_open_blob2(firebirdconn->error,
				&firebirdconn->db,
				&firebirdconn->tr,
				&outblobhandle[index],
				&outblobid[index],0,NULL)) {
		return false;
	}

	bool	retval=true;

	// read blob info
	char	blobitems[]={isc_info_blob_total_length};
	char	resultbuffer[64];
	if (isc_blob_info(firebirdconn->error,
				&outblobhandle[index],
				sizeof(blobitems),
				blobitems,
				sizeof(resultbuffer),
				resultbuffer)) {
		retval=false;
	}

	// get the blob size from the result buffer
	for (const char *p=resultbuffer; *p!=isc_info_end;) {

		// get the item type
		char	itemtype=*p;
		p++;

		// get the item size
		// (modern versions of isc_vax_integer take a const char *
		// parameter, but old versions take char * and this cast works
		// with both)
		uint16_t	itemsize=(uint16_t)isc_vax_integer((char *)p,2);
		p=p+2;

		// get the lob length
		if (itemtype==isc_info_blob_total_length) {
			// (modern versions of isc_vax_integer take a
			// const char * parameter, but old versions take a
			// char * and this cast works with both)
			*length=isc_vax_integer((char *)p,itemsize);
		}
 
		// move on
		p=p+itemsize;
	}
				
	// close the blob
	isc_close_blob(firebirdconn->error,&outblobhandle[index]);

	return retval;
}

bool firebirdcursor::getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

	// open the blob, if necessary
	if (!outblobisopen[index]) {
		outblobhandle[index]=NULL;
		if (isc_open_blob2(firebirdconn->error,
					&firebirdconn->db,
					&firebirdconn->tr,
					&outblobhandle[index],
					&outblobid[index],0,NULL)) {
			return false;
		}
		outblobisopen[index]=true;
	}

	// read a blob segment, at most MAX_LOB_CHUNK_SIZE bytes at a time
	uint64_t	totalbytesread=0;
	uint64_t	bytestoread=0;
	uint64_t	remainingbytestoread=charstoread;
	ISC_STATUS	status=0;
	for (;;) {

		// figure out how many bytes to read this time
		if (remainingbytestoread<MAX_LOB_CHUNK_SIZE) {
			bytestoread=remainingbytestoread;
		} else {
			bytestoread=MAX_LOB_CHUNK_SIZE;
			remainingbytestoread=remainingbytestoread-
						MAX_LOB_CHUNK_SIZE;
		}
		// read the bytes
		uint16_t	bytesread=0;
		status=isc_get_segment(firebirdconn->error,
					&outblobhandle[index],
					&bytesread,
					bytestoread,
					buffer+totalbytesread);

		// bail on error
		if (status && status!=isc_segment) {
			break;
		}

		// update total bytes read
		totalbytesread=totalbytesread+bytesread;

		// bail if we're done reading
		if (bytesread<bytestoread || totalbytesread==charstoread) {
			break;
		}
	}

	// return number of bytes/chars read
	*charsread=totalbytesread;

	return true;
}

void firebirdcursor::cleanUpLobOutputBind(uint16_t index) {

	// close the blob, if necessary
	if (outblobisopen[index]) {
		isc_close_blob(firebirdconn->error,&outblobhandle[index]);
		outblobisopen[index]=false;
	}
}

bool firebirdcursor::executeQuery(const char *query, uint32_t length) {

	// for commit or rollback, execute the API call and return
	if (querytype==isc_info_sql_stmt_commit) {
		return !isc_commit_retaining(firebirdconn->error,
							&firebirdconn->tr);
	} else if (querytype==isc_info_sql_stmt_rollback) {
		return !isc_rollback_retaining(firebirdconn->error,
							&firebirdconn->tr);
	} else if (queryisexecsp) {

		// if the query is a stored procedure then execute it as such
		bool	retval=!isc_dsql_execute2(firebirdconn->error,
						&firebirdconn->tr,
						&stmt,1,insqlda,outsqlda);

		for (short i=0; i<outsqlda->sqld; i++) {

			if (outsqlda->sqlvar[i].sqltype==SQL_TEXT+1) {

				// null-terminate strings
				outsqlda->sqlvar[i].
					sqldata[outsqlda->sqlvar[i].sqllen-1]=0;

			} else if (outsqlda->sqlvar[i].sqltype==SQL_TIMESTAMP) {

				// copy out date bind data
				tm	t;
				isc_decode_timestamp((ISC_TIMESTAMP *)
						outsqlda->sqlvar[i].sqldata,&t);
				*(outdatebind[i].year)=t.tm_year+1900;
				*(outdatebind[i].month)=t.tm_mon+1;
				*(outdatebind[i].day)=t.tm_mday;
				*(outdatebind[i].hour)=t.tm_hour;
				*(outdatebind[i].minute)=t.tm_min;
				*(outdatebind[i].second)=t.tm_sec;
				*(outdatebind[i].tz)=NULL;
			}
		}

		// set column count to 0
		outsqlda->sqld=0;
		return retval;
	}

	// handle non-stored procedures...

	// describe the cursor
	outsqlda->sqld=0;
	if (isc_dsql_describe(firebirdconn->error,&stmt,1,outsqlda)) {
		return false;
	}
	if (outsqlda->sqld>MAX_SELECT_LIST_SIZE) {
		outsqlda->sqld=MAX_SELECT_LIST_SIZE;
	}

	for (short i=0; i<outsqlda->sqld; i++) {

		// save the actual field type
		field[i].type=outsqlda->sqlvar[i].sqltype;

		// handle the null indicator
		outsqlda->sqlvar[i].sqlind=&field[i].nullindicator;

		// coerce the datatypes and point where the data should go
		if (outsqlda->sqlvar[i].sqltype==SQL_TEXT || 
				outsqlda->sqlvar[i].sqltype==SQL_TEXT+1) {
			outsqlda->sqlvar[i].sqldata=field[i].textbuffer;
			field[i].sqlrtype=CHAR_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_VARYING ||
				outsqlda->sqlvar[i].
					sqltype==SQL_VARYING+1) {
			outsqlda->sqlvar[i].sqldata=field[i].textbuffer;
			field[i].sqlrtype=VARCHAR_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_SHORT ||
				outsqlda->sqlvar[i].sqltype==SQL_SHORT+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].shortbuffer;
			field[i].sqlrtype=SMALLINT_DATATYPE;

		// Looks like sometimes firebird returns INT64's as
		// SQL_LONG type.  These can be identified because
		// the sqlscale gets set too.  Treat SQL_LONG's with
		// an sqlscale as INT64's.
		} else if ((outsqlda->sqlvar[i].sqltype==SQL_LONG ||
				outsqlda->sqlvar[i].sqltype==SQL_LONG+1) &&
				!outsqlda->sqlvar[i].sqlscale) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].longbuffer;
			field[i].sqlrtype=INTEGER_DATATYPE;
		} else if (
		#ifdef SQL_INT64
				(outsqlda->sqlvar[i].sqltype==SQL_INT64 ||
				outsqlda->sqlvar[i].sqltype==SQL_INT64+1) ||
		#endif
				((outsqlda->sqlvar[i].sqltype==SQL_LONG ||
				outsqlda->sqlvar[i].sqltype==SQL_LONG+1) &&
				outsqlda->sqlvar[i].sqlscale)) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].int64buffer;
			if (outsqlda->sqlvar[i].sqlsubtype==1) {
				field[i].sqlrtype=NUMERIC_DATATYPE;
			} else {
				field[i].sqlrtype=DECIMAL_DATATYPE;
			}
		} else if (outsqlda->sqlvar[i].sqltype==SQL_FLOAT ||
			outsqlda->sqlvar[i].sqltype==SQL_FLOAT+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].floatbuffer;
			field[i].sqlrtype=FLOAT_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_DOUBLE ||
			outsqlda->sqlvar[i].sqltype==SQL_DOUBLE+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].doublebuffer;
			field[i].sqlrtype=DOUBLE_PRECISION_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_D_FLOAT ||
			outsqlda->sqlvar[i].sqltype==SQL_D_FLOAT+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].doublebuffer;
			field[i].sqlrtype=D_FLOAT_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_ARRAY || 
				outsqlda->sqlvar[i].sqltype==SQL_ARRAY+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].quadbuffer;
			field[i].sqlrtype=ARRAY_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_QUAD || 
				outsqlda->sqlvar[i].sqltype==SQL_QUAD+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].quadbuffer;
			field[i].sqlrtype=QUAD_DATATYPE;
		#ifdef SQL_TIMESTAMP
		} else if (outsqlda->sqlvar[i].sqltype==SQL_TIMESTAMP || 
				outsqlda->sqlvar[i].
					sqltype==SQL_TIMESTAMP+1) {
		#else
		} else if (outsqlda->sqlvar[i].sqltype==SQL_DATE || 
				outsqlda->sqlvar[i].sqltype==SQL_DATE+1) {
		#endif
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].timestampbuffer;
			field[i].sqlrtype=TIMESTAMP_DATATYPE;
		#ifdef SQL_TIMESTAMP
		} else if (outsqlda->sqlvar[i].sqltype==SQL_TYPE_TIME || 
				outsqlda->sqlvar[i].
					sqltype==SQL_TYPE_TIME+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].timebuffer;
			field[i].sqlrtype=TIME_DATATYPE;
		} else if (outsqlda->sqlvar[i].sqltype==SQL_TYPE_DATE || 
				outsqlda->sqlvar[i].
					sqltype==SQL_TYPE_DATE+1) {
			outsqlda->sqlvar[i].sqldata=
					(char *)&field[i].datebuffer;
			field[i].sqlrtype=DATE_DATATYPE;
		#endif
		} else if (outsqlda->sqlvar[i].sqltype==SQL_BLOB || 
				outsqlda->sqlvar[i].sqltype==SQL_BLOB+1) {
			outsqlda->sqlvar[i].sqltype=SQL_BLOB;
			outsqlda->sqlvar[i].sqldata=(char *)&field[i].blobid;
			outsqlda->sqlvar[i].sqllen=sizeof(ISC_QUAD);
			field[i].sqlrtype=BLOB_DATATYPE;
			field[i].blobisopen=false;
		} else {
			outsqlda->sqlvar[i].sqltype=SQL_VARYING;
			outsqlda->sqlvar[i].sqldata=field[i].textbuffer;
			field[i].sqlrtype=UNKNOWN_DATATYPE;
		}
	}

	// Execute the query
	return !isc_dsql_execute(firebirdconn->error,&firebirdconn->tr,
							&stmt,1,insqlda);
}

void firebirdcursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	// handle bind format errors
	if (bindformaterror) {
		*errorlength=charstring::length(
				SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING);
		charstring::safeCopy(errorbuffer,
				errorbufferlength,
				SQLR_ERROR_INVALIDBINDVARIABLEFORMAT_STRING,
				*errorlength);
		*errorcode=SQLR_ERROR_INVALIDBINDVARIABLEFORMAT;
		*liveconnection=true;
		return;
	}

	// otherwise fall back to default implementation
	sqlrcursor_svr::errorMessage(errorbuffer,
					errorbufferlength,
					errorlength,
					errorcode,
					liveconnection);
}

bool firebirdcursor::queryIsNotSelect() {
	return (querytype!=isc_info_sql_stmt_select);
}

bool firebirdcursor::queryIsCommitOrRollback() {
	return (querytype==isc_info_sql_stmt_commit ||
		querytype==isc_info_sql_stmt_rollback);
}

bool firebirdcursor::knowsAffectedRows() {
	return false;
}

uint32_t firebirdcursor::colCount() {
	// for exec procedure queries, outsqlda contains output bind values
	// rather than column info and there is no result set, thus no column
	// info
	return outsqlda->sqld;
}

const char *firebirdcursor::getColumnName(uint32_t col) {
	return outsqlda->sqlvar[col].aliasname;
}

uint16_t firebirdcursor::getColumnNameLength(uint32_t col) {
	return outsqlda->sqlvar[col].aliasname_length;
}

uint16_t firebirdcursor::getColumnType(uint32_t col) {
	return field[col].sqlrtype;
}

uint32_t firebirdcursor::getColumnLength(uint32_t col) {
	return outsqlda->sqlvar[col].sqllen;
}

uint32_t firebirdcursor::getColumnPrecision(uint32_t col) {

	switch (field[col].sqlrtype) {
		case CHAR_DATATYPE:
			return outsqlda->sqlvar[col].sqllen;
		case VARCHAR_DATATYPE:
			return outsqlda->sqlvar[col].sqllen;
		case SMALLINT_DATATYPE:
			return 5;
		case INTEGER_DATATYPE:
			return 11;
		case NUMERIC_DATATYPE:
			// FIXME: can be from 1 to 18
			// (oddly, scale is given as a negative number)
			return 18+outsqlda->sqlvar[col].sqlscale;
		case DECIMAL_DATATYPE:
			// FIXME: can be from 1 to 18
			// (oddly, scale is given as a negative number)
			return 18+outsqlda->sqlvar[col].sqlscale;
		case FLOAT_DATATYPE:
			return 0;
		case DOUBLE_PRECISION_DATATYPE:
			return 0;
		case D_FLOAT_DATATYPE:
			return 0;
		case ARRAY_DATATYPE:
			// not sure
			return 0;
		case QUAD_DATATYPE:
			// not sure
			return 0;
		case TIMESTAMP_DATATYPE:
			// not sure
			return 0;
		case TIME_DATATYPE:
			return 8;
		case DATE_DATATYPE:
			return 10;
		case BLOB_DATATYPE:
			return outsqlda->sqlvar[col].sqllen;
		default:
			return outsqlda->sqlvar[col].sqllen;
	}
}

uint32_t firebirdcursor::getColumnScale(uint32_t col) {
	return -outsqlda->sqlvar[col].sqlscale;
}

bool firebirdcursor::noRowsToReturn() {
	// for exec procedure queries, outsqlda contains output bind values
	// rather than a result set and there is no result set
	return (queryisexecsp)?true:!outsqlda->sqld;
}

bool firebirdcursor::fetchRow() {

	ISC_STATUS	retcode;
	if ((retcode=isc_dsql_fetch(firebirdconn->error,
					&stmt,1,outsqlda))) {
		// if retcode is 100L, then there are no more rows,
		// otherwise, there is an error... how do I handle this?
		return false;
	}
	return true;
}

void firebirdcursor::getField(uint32_t col,
				const char **fld, uint64_t *fldlength,
				bool *blob, bool *null) {

	// handle a null field
	if ((outsqlda->sqlvar[col].sqltype & 1) && 
		field[col].nullindicator==-1) {
		*null=true;
		return;
	}


	// handle a non-null field
	if (outsqlda->sqlvar[col].sqltype==SQL_TEXT ||
			outsqlda->sqlvar[col].sqltype==SQL_TEXT+1) {
		size_t	maxlen=outsqlda->sqlvar[col].sqllen;
		size_t	reallen=charstring::length(field[col].textbuffer);
		if (reallen>maxlen) {
			reallen=maxlen;
		}
		*fld=field[col].textbuffer;
		*fldlength=reallen;
		return;
	} else if (outsqlda->sqlvar[col].
				sqltype==SQL_SHORT ||
			outsqlda->sqlvar[col].
				sqltype==SQL_SHORT+1) {
		fieldbuffer.clear();
		fieldbuffer.append(field[col].shortbuffer);
	} else if (outsqlda->sqlvar[col].
				sqltype==SQL_FLOAT ||
			outsqlda->sqlvar[col].
				sqltype==SQL_FLOAT+1) {
		fieldbuffer.clear();
		fieldbuffer.append((double)field[col].floatbuffer);
	} else if (outsqlda->sqlvar[col].
				sqltype==SQL_DOUBLE ||
			outsqlda->sqlvar[col].
				sqltype==SQL_DOUBLE+1 ||
			outsqlda->sqlvar[col].
				sqltype==SQL_D_FLOAT ||
			outsqlda->sqlvar[col].
				sqltype==SQL_D_FLOAT+1) {
		fieldbuffer.clear();
		fieldbuffer.append((double)field[col].doublebuffer);
	} else if (outsqlda->sqlvar[col].
				sqltype==SQL_VARYING ||
			outsqlda->sqlvar[col].
				sqltype==SQL_VARYING+1) {
		// the first 2 bytes are the length in 
		// an SQL_VARYING field
		int16_t	size;
		bytestring::copy((void *)&size,
				(void *)field[col].textbuffer,
				sizeof(int16_t));
		*fld=field[col].textbuffer+sizeof(int16_t);
		*fldlength=size;
		return;

	// Looks like sometimes firebird returns INT64's as
	// SQL_LONG type.  These can be identified because
	// the sqlscale gets set too.  Treat SQL_LONG's with
	// an sqlscale as INT64's.
	} else if ((outsqlda->sqlvar[col].
				sqltype==SQL_LONG ||
			outsqlda->sqlvar[col].
				sqltype==SQL_LONG+1) &&
			!outsqlda->sqlvar[col].sqlscale) {
		fieldbuffer.clear();
		fieldbuffer.append((int32_t)field[col].longbuffer);
	} else if (
	#ifdef SQL_INT64
			(outsqlda->sqlvar[col].
				sqltype==SQL_INT64 ||
			outsqlda->sqlvar[col].
				sqltype==SQL_INT64+1) ||
	#endif
			((outsqlda->sqlvar[col].
				sqltype==SQL_LONG ||
			outsqlda->sqlvar[col].
				sqltype==SQL_LONG+1) &&
			outsqlda->sqlvar[col].sqlscale)) {
		// int64's are weird.  To the left of the decimal
		// point is the value/10^scale, to the right is
		// value%10^scale
		fieldbuffer.clear();
		if (outsqlda->sqlvar[col].sqlscale) {

			fieldbuffer.append((int64_t)(field[col].int64buffer/(int)pow(10.0,(double)-outsqlda->sqlvar[col].sqlscale)))->append(".");

			stringbuffer	decimal;
			decimal.append((int64_t)(field[col].int64buffer%(int)pow(10.0,(double)-outsqlda->sqlvar[col].sqlscale)));
		
			// gotta get the right number
			// of decimal places
			for (int32_t i=charstring::length(
					decimal.getString());
				i<-outsqlda->sqlvar[col].sqlscale;
				i++) {
				decimal.append("0");
			}
			fieldbuffer.append(decimal.getString());
		} else {
			fieldbuffer.append((int64_t)field[col].int64buffer);
		}
	} else if (outsqlda->sqlvar[col].sqltype==SQL_ARRAY ||
		outsqlda->sqlvar[col].sqltype==SQL_ARRAY+1 ||
		outsqlda->sqlvar[col].sqltype==SQL_QUAD ||
		outsqlda->sqlvar[col].sqltype==SQL_QUAD+1) {
		// FIXME: handle arrays for real here...
		*null=true;
		return;
	#ifdef SQL_TIMESTAMP
	} else if (outsqlda->sqlvar[col].sqltype==SQL_TIMESTAMP ||
		outsqlda->sqlvar[col].sqltype==SQL_TIMESTAMP+1) {
		// decode the timestamp
		tm	entry_timestamp;
		isc_decode_timestamp(&field[col].timestampbuffer,
						&entry_timestamp);
	#else
	} else if (outsqlda->sqlvar[col].sqltype==SQL_DATE ||
		outsqlda->sqlvar[col].sqltype==SQL_DATE+1) {

		// decode the timestamp
		tm	entry_timestamp;
		isc_decode_date(&field[col].timestampbuffer,
						&entry_timestamp);
	#endif
		// build a string of "yyyy-mm-dd hh:mm:ss" format
		char	buffer[20];
		charstring::printf(buffer,20,
					"%d-%02d-%02d %02d:%02d:%02d",
					entry_timestamp.tm_year+1900,
					entry_timestamp.tm_mon+1,
					entry_timestamp.tm_mday,
					entry_timestamp.tm_hour,
					entry_timestamp.tm_min,
					entry_timestamp.tm_sec);
		fieldbuffer.clear();
		fieldbuffer.append(buffer);
	#ifdef SQL_TIMESTAMP
	} else if (outsqlda->sqlvar[col].sqltype==SQL_TYPE_TIME ||
		outsqlda->sqlvar[col].sqltype==SQL_TYPE_TIME+1) {
		// decode the time
		tm	entry_time;
		isc_decode_sql_time(&field[col].timebuffer,
						&entry_time);
		// build a string of "hh:mm:ss" format
		char	buffer[9];
		charstring::printf(buffer,9,
					"%02d:%02d:%02d",
					entry_time.tm_hour,
					entry_time.tm_min,
					entry_time.tm_sec);
		fieldbuffer.clear();
		fieldbuffer.append(buffer);
	} else if (outsqlda->sqlvar[col].sqltype==SQL_TYPE_DATE ||
		outsqlda->sqlvar[col].sqltype==SQL_TYPE_DATE+1) {
		// decode the date
		tm	entry_date;
		isc_decode_sql_date(&field[col].datebuffer,
						&entry_date);
		// build a string of "yyyy-mm-dd" format
		char	buffer[11];
		charstring::printf(buffer,11,
					"%d:%02d:%02d",
					entry_date.tm_year+1900,
					entry_date.tm_mon+1,
					entry_date.tm_mday);
		fieldbuffer.clear();
		fieldbuffer.append(buffer);
	#endif
	} else if (outsqlda->sqlvar[col].sqltype==SQL_BLOB ||
			outsqlda->sqlvar[col].sqltype==SQL_BLOB+1) {
		*blob=true;
		return;
	}

	// for any case that didn't return already, we need to do this...
	*fld=fieldbuffer.getString();
	*fldlength=fieldbuffer.getStringLength();
}

bool firebirdcursor::getLobFieldLength(uint32_t col, uint64_t *length) {

	// ignore non-blobs
	if (field[col].sqlrtype!=BLOB_DATATYPE) {
		return false;
	}

	// open the blob
	field[col].blobhandle=NULL;
	if (isc_open_blob2(firebirdconn->error,
				&firebirdconn->db,
				&firebirdconn->tr,
				&field[col].blobhandle,
				&field[col].blobid,0,NULL)) {
		return false;
	}

	bool	retval=true;

	// read blob info
	char	blobitems[]={isc_info_blob_total_length};
	char	resultbuffer[64];
	if (isc_blob_info(firebirdconn->error,
				&field[col].blobhandle,
				sizeof(blobitems),
				blobitems,
				sizeof(resultbuffer),
				resultbuffer)) {
		retval=false;
	}

	// get the blob size from the result buffer
	for (const char *p=resultbuffer; *p!=isc_info_end;) {

		// get the item type
		char	itemtype=*p;
		p++;

		// get the item size
		// (modern versions of isc_vax_integer take a const char *
		// parameter, but old versions take a char * and this cast
		// works with both)
		uint16_t	itemsize=(uint16_t)isc_vax_integer((char *)p,2);
		p=p+2;

		// get the lob length
		if (itemtype==isc_info_blob_total_length) {
			// (modern versions of isc_vax_integer take a
			// const char * parameter, but old versions take a
			// char * and this cast works with both)
			*length=isc_vax_integer((char *)p,itemsize);
		}
 
		// move on
		p=p+itemsize;
	}

	// close the blob
	isc_close_blob(firebirdconn->error,&field[col].blobhandle);

	return retval;
}

bool firebirdcursor::getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {

	// ignore non-blobs
	if (field[col].sqlrtype!=BLOB_DATATYPE) {
		return false;
	}

	// open the blob, if necessary
	if (!field[col].blobisopen) {
		field[col].blobhandle=NULL;
		if (isc_open_blob2(firebirdconn->error,
					&firebirdconn->db,
					&firebirdconn->tr,
					&field[col].blobhandle,
					&field[col].blobid,0,NULL)) {
			return false;
		}
		field[col].blobisopen=true;
	}

	// read a blob segment, at most MAX_LOB_CHUNK_SIZE bytes at a time
	uint64_t	totalbytesread=0;
	uint64_t	bytestoread=0;
	uint64_t	remainingbytestoread=charstoread;
	ISC_STATUS	status=0;
	for (;;) {

		// figure out how many bytes to read this time
		if (remainingbytestoread<MAX_LOB_CHUNK_SIZE) {
			bytestoread=remainingbytestoread;
		} else {
			bytestoread=MAX_LOB_CHUNK_SIZE;
			remainingbytestoread=remainingbytestoread-
						MAX_LOB_CHUNK_SIZE;
		}
		// read the bytes
		uint16_t	bytesread=0;
		status=isc_get_segment(firebirdconn->error,
					&field[col].blobhandle,
					&bytesread,
					bytestoread,
					buffer+totalbytesread);

		// bail on error
		if (status && status!=isc_segment) {
			break;
		}

		// update total bytes read
		totalbytesread=totalbytesread+bytesread;

		// bail if we're done reading
		if (bytesread<bytestoread || totalbytesread==charstoread) {
			break;
		}
	}

	// return number of bytes/chars read
	*charsread=totalbytesread;

	return true;
}

void firebirdcursor::cleanUpLobField(uint32_t col) {

	// ignore non-blobs
	if (field[col].sqlrtype!=BLOB_DATATYPE) {
		return;
	}

	// close the blob, if necessary
	if (field[col].blobisopen) {
		isc_close_blob(firebirdconn->error,&field[col].blobhandle);
		field[col].blobisopen=false;
	}
}

void firebirdcursor::cleanUpData() {
	outbindcount=0;
}

extern "C" {
	sqlrconnection_svr *new_firebirdconnection(sqlrcontroller_svr *cont) {
		return new firebirdconnection(cont);
	}
}
