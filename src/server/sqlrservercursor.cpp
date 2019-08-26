// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>

#define NEED_DATATYPESTRING 1
#include <datatypes.h>
#include <defines.h>
#define NEED_BEFORE_BIND_VARIABLE 1
#define NEED_AFTER_BIND_VARIABLE 1
#define NEED_IS_BIND_DELIMITER 1
#include <bindvariables.h>

class sqlrservercursorprivate {
	 friend class sqlrservercursor;

		regularexpression	_createtemp;

		uint16_t	_id;

		char			*_querybuffer;
		uint32_t		_querylength;
		sqlrquerystatus_t	_querystatus;
		stringbuffer		_querywithfakeinputbinds;

		xmldom		*_querytree;
		stringbuffer	_translatedquery;

		memorypool	_bindpool;
		memorypool	_bindmappingspool;
		namevaluepairs	*_bindmappings;

		uint16_t		_inbindcount;
		sqlrserverbindvar	*_inbindvars;
		uint16_t		_outbindcount;
		sqlrserverbindvar	*_outbindvars;
		uint16_t		_inoutbindcount;
		sqlrserverbindvar	*_inoutbindvars;

		uint64_t	_totalrowsfetched;

		bool		_currentrowreformatted;

		uint64_t	_commandstartsec;
		uint64_t	_commandstartusec;
		uint64_t	_commandendsec;
		uint64_t	_commandendusec;
		uint64_t	_querystartsec;
		uint64_t	_querystartusec;
		uint64_t	_queryendsec;
		uint64_t	_queryendusec;
		uint64_t	_fetchstartsec;
		uint64_t	_fetchstartusec;
		uint64_t	_fetchendsec;
		uint64_t	_fetchendusec;
		uint64_t	_fetchusec;

		uint32_t	_maxerrorlength;

		char		*_error;
		uint32_t	_errorlength;
		int64_t		_errnum;
		bool		_liveconnection;

		sqlrcursorstate_t	_state;

		sqlrquerycursor		*_customquerycursor;

		bool	_columninfoisvalid;

		bool	_queryhasbeenpreprocessed;
		bool	_queryhasbeenprepared;
		bool	_queryhasbeenexecuted;
		bool	_queryneedsintercept;
		bool	_querywasintercepted;
		bool	_bindswerefaked;
		bool	_fakeinputbindsforthisquery;
		sqlrquerytype_t	_querytype;

		const char	**_columnnames;
		uint16_t	*_columnnamelengths;
		uint16_t	*_columntypes;
		const char	**_columntypenames;
		uint16_t	*_columntypenamelengths;
		uint32_t	*_columnlengths;
		uint32_t	*_columnprecisions;
		uint32_t	*_columnscales;
		uint16_t	*_columnisnullables;
		uint16_t	*_columnisprimarykeys;
		uint16_t	*_columnisuniques;
		uint16_t	*_columnispartofkeys;
		uint16_t	*_columnisunsigneds;
		uint16_t	*_columniszerofilleds;
		uint16_t	*_columnisbinarys;
		uint16_t	*_columnisautoincrements;
		const char	**_columntables;
		uint16_t	*_columntablelengths;

		const char	**_fieldnames;
		const char	**_fields;
		uint64_t	*_fieldlengths;
		bool		*_blobs;
		bool		*_nulls;

		uint64_t	_querytimeout;
		bool		_executedirect;
		bool		_executerpc;

		bool		_resultsetheaderhasbeenhandled;

		unsigned char	_moduledata[1024];
};

sqlrservercursor::sqlrservercursor(sqlrserverconnection *conn, uint16_t id) {

	pvt=new sqlrservercursorprivate;

	this->conn=conn;

	pvt->_maxerrorlength=conn->cont->getConfig()->getMaxErrorLength();

	pvt->_bindmappings=new namevaluepairs;

	setInputBindCount(0);
	pvt->_inbindvars=new sqlrserverbindvar[
				conn->cont->getConfig()->getMaxBindCount()];
	setOutputBindCount(0);
	pvt->_outbindvars=new sqlrserverbindvar[
				conn->cont->getConfig()->getMaxBindCount()];
	setInputOutputBindCount(0);
	pvt->_inoutbindvars=new sqlrserverbindvar[
				conn->cont->getConfig()->getMaxBindCount()];

	pvt->_totalrowsfetched=0;

	pvt->_currentrowreformatted=false;

	pvt->_commandstartsec=0;
	pvt->_commandstartusec=0;
	pvt->_commandendsec=0;
	pvt->_commandendusec=0;
	pvt->_querystartsec=0;
	pvt->_querystartusec=0;
	pvt->_queryendsec=0;
	pvt->_queryendusec=0;
	pvt->_fetchstartsec=0;
	pvt->_fetchstartusec=0;
	pvt->_fetchendsec=0;
	pvt->_fetchendusec=0;
	pvt->_fetchusec=0;
	
	setState(SQLRCURSORSTATE_AVAILABLE);

	setCreateTempTablePattern("(create|CREATE|declare|DECLARE)[ 	\\r\\n]+((global|GLOBAL|local|LOCAL)?[ 	\\r\\n]+)?(temp|TEMP|temporary|TEMPORARY)?[ 	\\r\\n]+(table|TABLE)[ 	\\r\\n]+");

	pvt->_querybuffer=
		new char[conn->cont->getConfig()->getMaxQuerySize()+1];
	setQueryLength(0);

	setQueryStatus(SQLRQUERYSTATUS_ERROR);

	setQueryTree(NULL);

	pvt->_error=new char[pvt->_maxerrorlength+1];
	pvt->_errorlength=0;
	pvt->_errnum=0;
	pvt->_liveconnection=true;

	setCommandStart(0,0);
	setCommandEnd(0,0);
	setQueryStart(0,0);
	setQueryEnd(0,0);

	setCustomQueryCursor(NULL);

	clearTotalRowsFetched();

	pvt->_id=id;

	pvt->_columninfoisvalid=false;

	pvt->_queryhasbeenpreprocessed=false;
	pvt->_queryhasbeenprepared=false;
	pvt->_queryhasbeenexecuted=false;
	pvt->_queryneedsintercept=false;
	pvt->_querywasintercepted=false;
	pvt->_bindswerefaked=false;
	pvt->_fakeinputbindsforthisquery=false;
	pvt->_querytype=SQLRQUERYTYPE_ETC;

	pvt->_columnnames=NULL;
	pvt->_columnnamelengths=NULL;
	pvt->_columntypes=NULL;
	pvt->_columntypenames=NULL;
	pvt->_columntypenamelengths=NULL;
	pvt->_columnlengths=NULL;
	pvt->_columnprecisions=NULL;
	pvt->_columnscales=NULL;
	pvt->_columnisnullables=NULL;
	pvt->_columnisprimarykeys=NULL;
	pvt->_columnisuniques=NULL;
	pvt->_columnispartofkeys=NULL;
	pvt->_columnisunsigneds=NULL;
	pvt->_columniszerofilleds=NULL;
	pvt->_columnisbinarys=NULL;
	pvt->_columnisautoincrements=NULL;
	pvt->_columntables=NULL;
	pvt->_columntablelengths=NULL;

	pvt->_fieldnames=NULL;
	pvt->_fields=NULL;
	pvt->_fieldlengths=NULL;
	pvt->_blobs=NULL;
	pvt->_nulls=NULL;
	uint32_t	colcount=conn->cont->getMaxColumnCount();
	if (colcount) {
		allocateColumnPointers(colcount);
		allocateFieldPointers(colcount);
	}

	pvt->_querytimeout=conn->cont->getQueryTimeout();
	pvt->_executedirect=conn->cont->getExecuteDirect();
	pvt->_executerpc=false;

	pvt->_resultsetheaderhasbeenhandled=false;
}

sqlrservercursor::~sqlrservercursor() {
	delete[] pvt->_querybuffer;
	delete pvt->_querytree;
	delete pvt->_bindmappings;
	delete[] pvt->_inbindvars;
	delete[] pvt->_outbindvars;
	delete[] pvt->_inoutbindvars;
	delete pvt->_customquerycursor;
	delete[] pvt->_error;
	deallocateColumnPointers();
	deallocateFieldPointers();
	delete pvt;
}

bool sqlrservercursor::open() {
	return true;
}

bool sqlrservercursor::close() {
	// by default do nothing
	return true;
}

sqlrquerytype_t sqlrservercursor::queryType(const char *query,
						uint32_t length) {

	// skip past leading garbage
	const char	*ptr=conn->cont->skipWhitespaceAndComments(query);

	// initialize to "etc"
	sqlrquerytype_t	retval=SQLRQUERYTYPE_ETC;

	// look for specific query types
	if (!charstring::compare(ptr,"select",6)) {
		retval=SQLRQUERYTYPE_SELECT;
		ptr=ptr+6;
	} else if (!charstring::compare(ptr,"insert",6)) {
		retval=SQLRQUERYTYPE_INSERT;
		ptr=ptr+6;
	} else if (!charstring::compare(ptr,"update",6)) {
		retval=SQLRQUERYTYPE_UPDATE;
		ptr=ptr+6;
	} else if (!charstring::compare(ptr,"delete",6)) {
		retval=SQLRQUERYTYPE_DELETE;
		ptr=ptr+6;
	} else if (!charstring::compare(ptr,"create",6)) {
		retval=SQLRQUERYTYPE_CREATE;
		ptr=ptr+6;
	} else if (!charstring::compare(ptr,"drop",4)) {
		retval=SQLRQUERYTYPE_DROP;
		ptr=ptr+4;
	} else if (!charstring::compare(ptr,"alter",5)) {
		retval=SQLRQUERYTYPE_ALTER;
		ptr=ptr+5;
	} else if (!charstring::compare(ptr,"begin") ||
			!charstring::compare(ptr,"begin work")) {
		return SQLRQUERYTYPE_BEGIN;
	} else if (!charstring::compare(ptr,"start",5)) {
		retval=SQLRQUERYTYPE_BEGIN;
		ptr=ptr+5;
	} else if (!charstring::compare(ptr,"commit",6)) {
		return SQLRQUERYTYPE_COMMIT;
	} else if (!charstring::compare(ptr,"rollback",8)) {
		return SQLRQUERYTYPE_ROLLBACK;
	}

	// for some query types, verify that whitespace follows
	if (retval!=SQLRQUERYTYPE_ETC) {
		if (!character::isWhitespace(*ptr)) {
			retval=SQLRQUERYTYPE_ETC;
		}
	}
	return retval;
}

bool sqlrservercursor::isCustomQuery() {
	return false;
}

bool sqlrservercursor::prepareQuery(const char *query, uint32_t length) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::supportsNativeBinds(const char *query,
						uint32_t length) {
	return true;
}

bool sqlrservercursor::inputBind(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::inputBind(const char *variable,
					uint16_t variablesize,
					int64_t *value) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::inputBind(const char *variable,
					uint16_t variablesize,
					double *value, 
					uint32_t precision,
					uint32_t scale) {
	// by default, do nothing...
	return true;
}

void sqlrservercursor::dateToString(char *buffer, uint16_t buffersize,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, const char *tz,
				bool isnegative) {

	const char	*format=conn->cont->getConfig()->
					getFakeInputBindVariablesDateFormat();
	if (charstring::isNullOrEmpty(format)) {
		format="YYYY-MM-DD HH:MI:SS";
	}

	// FIXME: it'd be nice if we could pass buffer/buffersize
	// into convertDateTime
	char	*newdate=conn->cont->convertDateTime(format,
						year,month,day,
						hour,minute,second,
						microsecond,isnegative);
	charstring::safeCopy(buffer,buffersize,newdate);
	delete[] newdate;
}

bool sqlrservercursor::inputBind(const char *variable,
					uint16_t variablesize,
					int64_t year,
					int16_t month,
					int16_t day,
					int16_t hour,
					int16_t minute,
					int16_t second,
					int32_t microsecond,
					const char *tz,
					bool isnegative,
					char *buffer,
					uint16_t buffersize,
					int16_t *isnull) {
	dateToString(buffer,buffersize,year,month,day,
			hour,minute,second,microsecond,tz,isnegative);
	if (buffer[0]=='\0') {
		*isnull=conn->nullBindValue();
	}
	return inputBind(variable,variablesize,buffer,buffersize,isnull);
}

bool sqlrservercursor::inputBindBlob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	// fall back to string bind implementation
	return inputBind(variable,variablesize,value,valuesize,isnull);
}

bool sqlrservercursor::inputBindClob(const char *variable,
					uint16_t variablesize,
					const char *value,
					uint32_t valuesize,
					int16_t *isnull) {
	// fall back to string bind implementation
	return inputBind(variable,variablesize,value,valuesize,isnull);
}

bool sqlrservercursor::outputBind(const char *variable,
					uint16_t variablesize,
					char *value, 
					uint32_t valuesize, 
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::outputBind(const char *variable,
					uint16_t variablesize,
					int64_t *value,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::outputBind(const char *variable,
					uint16_t variablesize,
					double *value,
					uint32_t *precision,
					uint32_t *scale,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::outputBind(const char *variable,
					uint16_t variablesize,
					int16_t *year,
					int16_t *month,
					int16_t *day,
					int16_t *hour,
					int16_t *minute,
					int16_t *second,
					int32_t *microsecond,
					const char **tz,
					bool *isnegative,
					char *buffer,
					uint16_t buffersize,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::outputBindBlob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::outputBindClob(const char *variable,
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::outputBindCursor(const char *variable,
					uint16_t variablesize,
					sqlrservercursor *cursor) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	*length=0;
	return true;
}

bool sqlrservercursor::getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {
	*charsread=0;
	return false;
}

void sqlrservercursor::closeLobOutputBind(uint16_t index) {
	// by default, do nothing
}

bool sqlrservercursor::inputOutputBind(const char *variable,
						uint16_t variablesize,
						char *value, 
						uint32_t valuesize, 
						int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::inputOutputBind(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::inputOutputBind(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::inputOutputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						bool *isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::inputOutputBindBlob(const char *variable,
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::inputOutputBindClob(const char *variable,
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::getLobInputOutputBindLength(uint16_t index,
							uint64_t *length) {
	*length=0;
	return true;
}

bool sqlrservercursor::getLobInputOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {
	*charsread=0;
	return false;
}

void sqlrservercursor::closeLobInputOutputBind(uint16_t index) {
	// by default, do nothing
}

void sqlrservercursor::checkForTempTable(const char *query, uint32_t length) {

	// see if the query matches the pattern for a temporary query that
	// creates a temporary table
	const char	*ptr=skipCreateTempTableClause(query);
	if (!ptr) {
		return;
	}

	// get the table name
	stringbuffer	tablename;
	const char	*endptr=query+length;
	while (*ptr && !character::isWhitespace(*ptr) && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	// append to list of temp tables
	conn->cont->addSessionTempTableForDrop(tablename.getString());
}

const char *sqlrservercursor::truncateTableQuery() {
	return "delete from";
}

bool sqlrservercursor::executeQuery(const char *query, uint32_t length) {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::fetchFromBindCursor() {
	// by default, do nothing...
	return true;
}

bool sqlrservercursor::nextResultSet(bool *nextresultsetavailable) {
	// by default, a next result set is not available
	*nextresultsetavailable = false;
	return true;
}

bool sqlrservercursor::queryIsNotSelect() {

	// scan the query, bypassing whitespace and comments.
	const char	*ptr=
		conn->cont->skipWhitespaceAndComments(pvt->_querybuffer);

	// if the query is a select but not a select into then return false,
	// otherwise return true
	if (!charstring::compareIgnoringCase(ptr,"select",6) && 
		charstring::compareIgnoringCase(ptr,"select into ",12)) {
		return false;
	}
	return true;
}

bool sqlrservercursor::queryIsCommitOrRollback() {

	// scan the query, bypassing whitespace and comments.
	const char	*ptr=
		conn->cont->skipWhitespaceAndComments(pvt->_querybuffer);

	// if the query is a commit or rollback, return true
	// otherwise return false
	return (!charstring::compareIgnoringCase(ptr,"commit",6) ||
			!charstring::compareIgnoringCase(ptr,"rollback",8));
}

void sqlrservercursor::errorMessage(char *errorbuffer,
					uint32_t errorbuffersize,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	// if the cursor happens to have an error, then return that
	if (pvt->_errorlength) {
		charstring::safeCopy(errorbuffer,errorbuffersize,
					pvt->_error,pvt->_errorlength);
		*errorlength=pvt->_errorlength;
		if (*errorlength>errorbuffersize) {
			*errorlength=errorbuffersize;
		}
		*errorcode=pvt->_errnum;
		*liveconnection=pvt->_liveconnection;
	}

	// otherwise return the connection's error
	conn->errorMessage(errorbuffer,errorbuffersize,
				errorlength,errorcode,liveconnection);
}

bool sqlrservercursor::knowsRowCount() {
	return false;
}

uint64_t sqlrservercursor::rowCount() {
	return 0;
}

bool sqlrservercursor::knowsAffectedRows() {
	return true;
}

uint64_t sqlrservercursor::affectedRows() {
	return 0;
}

uint32_t sqlrservercursor::colCount() {
	return 0;
}

uint16_t sqlrservercursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_IDS;
}

const char *sqlrservercursor::getColumnName(uint32_t col) {
	return NULL;
}

uint16_t sqlrservercursor::getColumnNameLength(uint32_t col) {
	return charstring::length(getColumnName(col));
}

uint16_t sqlrservercursor::getColumnType(uint32_t col) {
	return UNKNOWN_DATATYPE;
}

const char *sqlrservercursor::getColumnTypeName(uint32_t col) {
	return datatypestring[getColumnType(col)];
}

uint16_t sqlrservercursor::getColumnTypeNameLength(uint32_t col) {
	return charstring::length(getColumnTypeName(col));
}

uint32_t sqlrservercursor::getColumnLength(uint32_t col) {
	return 0;
}

uint32_t sqlrservercursor::getColumnPrecision(uint32_t col) {
	return 0;
}

uint32_t sqlrservercursor::getColumnScale(uint32_t col) {
	return 0;
}

uint16_t sqlrservercursor::getColumnIsNullable(uint32_t col) {
	return 0;
}

uint16_t sqlrservercursor::getColumnIsPrimaryKey(uint32_t col) {
	return 0;
}

uint16_t sqlrservercursor::getColumnIsUnique(uint32_t col) {
	return 0;
}

uint16_t sqlrservercursor::getColumnIsPartOfKey(uint32_t col) {
	return 0;
}

uint16_t sqlrservercursor::getColumnIsUnsigned(uint32_t col) {
	return 0;
}

uint16_t sqlrservercursor::getColumnIsZeroFilled(uint32_t col) {
	return 0;
}

uint16_t sqlrservercursor::getColumnIsBinary(uint32_t col) {
	return 0;
}

uint16_t sqlrservercursor::getColumnIsAutoIncrement(uint32_t col) {
	return 0;
}

const char *sqlrservercursor::getColumnTable(uint32_t col) {
	return NULL;
}

uint16_t sqlrservercursor::getColumnTableLength(uint32_t col) {
	return charstring::length(getColumnTable(col));
}

bool sqlrservercursor::ignoreDateDdMmParameter(uint32_t col,
					const char *data, uint32_t size) {
	return false;
}

bool sqlrservercursor::noRowsToReturn() {
	return true;
}

bool sqlrservercursor::skipRow(bool *error) {
	return fetchRow(error);
}

bool sqlrservercursor::fetchRow(bool *error) {
	*error=false;
	// by default, indicate that we are at the end of the result set
	return false;
}

void sqlrservercursor::nextRow() {
	// by default, do nothing
}

void sqlrservercursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {
	// by default, do nothing
	*field=NULL;
	*fieldlength=0;
	*blob=false;
	*null=false;
}

bool sqlrservercursor::getLobFieldLength(uint32_t col, uint64_t *length) {
	*length=0;
	return true;
}

bool sqlrservercursor::getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {
	*charsread=0;
	return false;
}

void sqlrservercursor::closeLobField(uint32_t col) {
	// by default, do nothing
}

void sqlrservercursor::closeResultSet() {
	// by default, do nothing...
	return;
}

uint16_t sqlrservercursor::getId() {
	return pvt->_id;
}

bool sqlrservercursor::fakeInputBinds() {

	// return false if there aren't any input binds
	if (!pvt->_inbindcount) {
		return false;
	}

	// re-init the buffer
	pvt->_querywithfakeinputbinds.clear();

	// loop through the query, performing substitutions
	queryparsestate_t	parsestate=IN_QUERY;

	// use 1-based index for bind variables
	int64_t		bindindex=1;

	// run through the querybuffer...
	char		*ptr=pvt->_querybuffer;
	const char	*endptr=pvt->_querybuffer+pvt->_querylength;
	char		prev='\0';
	do {

		// if we're in the query...
		if (parsestate==IN_QUERY) {

			// if we find a quote, we're in quotes
			if (*ptr=='\'') {
				parsestate=IN_QUOTES;
			}

			// if we find whitespace or a couple of other things
			// then the next thing could be a bind variable
			if (beforeBindVariable(ptr)) {
				parsestate=BEFORE_BIND;
			}

			// append the character
			pvt->_querywithfakeinputbinds.append(*ptr);

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		// copy anything in quotes verbatim
		if (parsestate==IN_QUOTES) {

			// if we find a quote, but not an escaped quote,
			// then we're back in the query
			// (or we're in between one of these: '...''...'
			// which is functionally the same)
			if (*ptr=='\'' && prev!='\\') {
				parsestate=IN_QUERY;
			}

			// append the character
			pvt->_querywithfakeinputbinds.append(*ptr);

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		if (parsestate==BEFORE_BIND) {

			// if we find a bind variable...
			if (isBindDelimiter(ptr,
				conn->cont->getConfig()->
				getBindVariableDelimiterQuestionMarkSupported(),
				conn->cont->getConfig()->
				getBindVariableDelimiterColonSupported(),
				conn->cont->getConfig()->
				getBindVariableDelimiterAtSignSupported(),
				conn->cont->getConfig()->
				getBindVariableDelimiterDollarSignSupported())
				) {
				parsestate=IN_BIND;
				continue;
			}

			// if we didn't find a bind variable then we're just
			// back in the query
			parsestate=IN_QUERY;
			continue;
		}

		// if we're in a bind variable...
		if (parsestate==IN_BIND) {

			// look through the list of vars
			for (int16_t i=0; i<pvt->_inbindcount; i++) {

				// if we find a match, perform the substitution
				// and skip past the variable...
				//
				// for bind-by-name cases, we need to check
				// after the variable for whitespace, a comma,
				// a semicolon, or a right parenthesis to make
				// sure that we don't make the following
				// mistake:
				//
				// select :var1,:var15 from mytable
				//
				// :var1=3
				// :var15=2
				//
				// correct:
				//      select 3,2 from mytable
				//
				// mistake:
				//      select 3,35 from mytable
				if (
					(*ptr=='?' && 
					charstring::toInteger(
						pvt->_inbindvars[i].
							variable+1)==bindindex) 

					||

					(!charstring::compare(ptr+1,
						pvt->_inbindvars[i].
							variable+1,
						pvt->_inbindvars[i].
							variablesize-1) 
					&&
					(afterBindVariable(
						ptr+pvt->_inbindvars[i].
							variablesize) ||
					ptr+1+pvt->_inbindvars[i].
						variablesize-1==endptr))) {

					performSubstitution(
					&(pvt->_querywithfakeinputbinds),i);
					if (*ptr=='?') {
						ptr++;
					} else {
						ptr=ptr+pvt->_inbindvars[i].
								variablesize;
					}
					bindindex++;
					break;
				}
			}

			parsestate=IN_QUERY;
			continue;
		}

	} while (ptr<endptr);

	return true;
}

void sqlrservercursor::performSubstitution(stringbuffer *buffer,
							int16_t index) {

	if (pvt->_inbindvars[index].type==SQLRSERVERBINDVARTYPE_STRING ||
		pvt->_inbindvars[index].type==SQLRSERVERBINDVARTYPE_CLOB) {

		if (conn->cont->getConfig()->
			getFakeInputBindVariablesUnicodeStrings()) {
			buffer->append('N');
		}

		buffer->append('\'');

		size_t	length=pvt->_inbindvars[index].valuesize;

		for (size_t ind=0; ind<length; ind++) {

			char	ch=pvt->_inbindvars[index].value.stringval[ind];

			// escape quotes and NULL's
			if (ch=='\'') {
				buffer->append('\'');
			} else if (ch=='\0') {
				// FIXME: I assume this is how NULL's should
				// be escaped, but I'm not really sure for
				// all db's...
				buffer->append("\\0");
			}
			buffer->append(ch);
		}

		buffer->append('\'');

	} else if (pvt->_inbindvars[index].type==
				SQLRSERVERBINDVARTYPE_BLOB) {
		encodeBlob(buffer,pvt->_inbindvars[index].value.stringval,
					pvt->_inbindvars[index].valuesize);
	} else if (pvt->_inbindvars[index].type==
				SQLRSERVERBINDVARTYPE_INTEGER) {
		buffer->append(pvt->_inbindvars[index].
					value.integerval);
	} else if (pvt->_inbindvars[index].type==
				SQLRSERVERBINDVARTYPE_DOUBLE) {
		char	*dbuf=NULL;
		if (!pvt->_inbindvars[index].
				value.doubleval.precision &&
			!pvt->_inbindvars[index].
				value.doubleval.scale) {
			dbuf=charstring::parseNumber(
				pvt->_inbindvars[index].
					value.doubleval.value);
		} else {
			dbuf=charstring::parseNumber(
				pvt->_inbindvars[index].
					value.doubleval.value,
				pvt->_inbindvars[index].
					value.doubleval.precision,
				pvt->_inbindvars[index].
					value.doubleval.scale);
		}
		// In some regions a comma is used rather than a period for
		// the decimal and the i8n settings will cause the vsnprintf 
		// that ultimately gets called by charstring::parseNumber above
		// to use a comma as the separator.  Databases don't like
		// commas in their numbers.  Convert commas to periods here.
		for (char *ptr=dbuf; *ptr; ptr++) {
			if (*ptr==',') {
				*ptr='.';
			}
		}
		buffer->append(dbuf);
		delete[] dbuf;
	} else if (pvt->_inbindvars[index].type==
				SQLRSERVERBINDVARTYPE_DATE) {
		char	buf[64];
		dateToString(buf,sizeof(buf),
			pvt->_inbindvars[index].value.dateval.year,
			pvt->_inbindvars[index].value.dateval.month,
			pvt->_inbindvars[index].value.dateval.day,
			pvt->_inbindvars[index].value.dateval.hour,
			pvt->_inbindvars[index].value.dateval.minute,
			pvt->_inbindvars[index].value.dateval.second,
			pvt->_inbindvars[index].value.dateval.microsecond,
			pvt->_inbindvars[index].value.dateval.tz,
			pvt->_inbindvars[index].value.dateval.isnegative);
		buffer->append("'")->append(buf)->append("'");
	} else if (pvt->_inbindvars[index].type==
				SQLRSERVERBINDVARTYPE_NULL) {
		buffer->append("NULL");
	}
}

void sqlrservercursor::encodeBlob(stringbuffer *buffer,
				const char *data, uint32_t datasize) {

	// by default, follow the SQL Standard:
	// X'...' where ... is the blob data and each byte of blob data is
	// converted to two hex characters..
	// eg: hello -> X'68656C6C6F'

	buffer->append("X\'");
	for (uint32_t i=0; i<datasize; i++) {
		buffer->append(conn->cont->asciiToHex(data[i]));
	}
	buffer->append('\'');
}

memorypool *sqlrservercursor::getBindPool() {
	return &pvt->_bindpool;
}

namevaluepairs *sqlrservercursor::getBindMappings() {
	return pvt->_bindmappings;
}

memorypool *sqlrservercursor::getBindMappingsPool() {
	return &pvt->_bindmappingspool;
}

void sqlrservercursor::setInputBindCount(uint16_t inbindcount) {
	pvt->_inbindcount=inbindcount;
}

uint16_t sqlrservercursor::getInputBindCount() {
	return pvt->_inbindcount;
}

sqlrserverbindvar *sqlrservercursor::getInputBinds() {
	return pvt->_inbindvars;
}

void sqlrservercursor::setOutputBindCount(uint16_t outbindcount) {
	pvt->_outbindcount=outbindcount;
}

uint16_t sqlrservercursor::getOutputBindCount() {
	return pvt->_outbindcount;
}

sqlrserverbindvar *sqlrservercursor::getOutputBinds() {
	return pvt->_outbindvars;
}

void sqlrservercursor::setInputOutputBindCount(uint16_t inoutbindcount) {
	pvt->_inoutbindcount=inoutbindcount;
}

uint16_t sqlrservercursor::getInputOutputBindCount() {
	return pvt->_inoutbindcount;
}

sqlrserverbindvar *sqlrservercursor::getInputOutputBinds() {
	return pvt->_inoutbindvars;
}

void sqlrservercursor::abort() {
	// I was once concerned that calling this here would prevent suspended
	// result sets from being able to return column data upon resume if the
	// entire result set had already been sent, but I don't think that's an
	// issue any more.
	closeResultSet();
	setState(SQLRCURSORSTATE_AVAILABLE);
	clearCustomQueryCursor();
}

char *sqlrservercursor::getQueryBuffer() {
	return pvt->_querybuffer;
}

uint32_t sqlrservercursor::getQueryLength() {
	return pvt->_querylength;
}

void sqlrservercursor::setQueryLength(uint32_t querylength) {
	pvt->_querylength=querylength;
}

void sqlrservercursor::setQueryStatus(sqlrquerystatus_t status) {
	pvt->_querystatus=status;
}

sqlrquerystatus_t sqlrservercursor::getQueryStatus() {
	return pvt->_querystatus;
}

void sqlrservercursor::setQueryTree(xmldom *tree) {
	pvt->_querytree=tree;
}

xmldom *sqlrservercursor::getQueryTree() {
	return pvt->_querytree;
}

void sqlrservercursor::clearQueryTree() {
	delete pvt->_querytree;
	pvt->_querytree=NULL;
}

stringbuffer *sqlrservercursor::getTranslatedQueryBuffer() {
	return &(pvt->_translatedquery);
}

void sqlrservercursor::setCommandStart(uint64_t sec, uint64_t usec) {
	pvt->_commandstartsec=sec;
	pvt->_commandstartusec=usec;
}

uint64_t sqlrservercursor::getCommandStartSec() {
	return pvt->_commandstartsec;
}

uint64_t sqlrservercursor::getCommandStartUSec() {
	return pvt->_commandstartusec;
}


void sqlrservercursor::setCommandEnd(uint64_t sec, uint64_t usec) {
	pvt->_commandendsec=sec;
	pvt->_commandendusec=usec;
}

uint64_t sqlrservercursor::getCommandEndSec() {
	return pvt->_commandendsec;
}

uint64_t sqlrservercursor::getCommandEndUSec() {
	return pvt->_commandendusec;
}


void sqlrservercursor::setQueryStart(uint64_t sec, uint64_t usec) {
	pvt->_querystartsec=sec;
	pvt->_querystartusec=usec;
}

uint64_t sqlrservercursor::getQueryStartSec() {
	return pvt->_querystartsec;
}

uint64_t sqlrservercursor::getQueryStartUSec() {
	return pvt->_querystartusec;
}


void sqlrservercursor::setQueryEnd(uint64_t sec, uint64_t usec) {
	pvt->_queryendsec=sec;
	pvt->_queryendusec=usec;
}

uint64_t sqlrservercursor::getQueryEndSec() {
	return pvt->_queryendsec;
}

uint64_t sqlrservercursor::getQueryEndUSec() {
	return pvt->_queryendusec;
}

void sqlrservercursor::setFetchStart(uint64_t sec, uint64_t usec) {
	pvt->_fetchstartsec=sec;
	pvt->_fetchstartusec=usec;
}

uint64_t sqlrservercursor::getFetchStartSec() {
	return pvt->_fetchstartsec;
}

uint64_t sqlrservercursor::getFetchStartUSec() {
	return pvt->_fetchstartusec;
}


void sqlrservercursor::setFetchEnd(uint64_t sec, uint64_t usec) {
	pvt->_fetchendsec=sec;
	pvt->_fetchendusec=usec;
}

uint64_t sqlrservercursor::getFetchEndSec() {
	return pvt->_fetchendsec;
}

uint64_t sqlrservercursor::getFetchEndUSec() {
	return pvt->_fetchendusec;
}

void sqlrservercursor::resetFetchTime() {
	pvt->_fetchusec=0;
}

void sqlrservercursor::tallyFetchTime() {
        pvt->_fetchusec+=((pvt->_fetchendsec-pvt->_fetchstartsec)*1000000)+
					pvt->_fetchendusec-pvt->_fetchstartusec;
}

uint64_t sqlrservercursor::getFetchUSec() {
	return pvt->_fetchusec;
}

void sqlrservercursor::setState(sqlrcursorstate_t state) {
	pvt->_state=state;
}

sqlrcursorstate_t sqlrservercursor::getState() {
	return pvt->_state;
}

void sqlrservercursor::setCustomQueryCursor(sqlrquerycursor *cur) {
	pvt->_customquerycursor=cur;
}

sqlrquerycursor *sqlrservercursor::getCustomQueryCursor() {
	return pvt->_customquerycursor;
}

void sqlrservercursor::clearCustomQueryCursor() {
	delete pvt->_customquerycursor;
	pvt->_customquerycursor=NULL;
}

void sqlrservercursor::clearTotalRowsFetched() {
	pvt->_totalrowsfetched=0;
}

uint64_t sqlrservercursor::getTotalRowsFetched() {
	return pvt->_totalrowsfetched;
}

void sqlrservercursor::incrementTotalRowsFetched() {
	pvt->_totalrowsfetched++;
}

void sqlrservercursor::setCurrentRowReformatted(bool crr) {
	pvt->_currentrowreformatted=crr;
}

bool sqlrservercursor::getCurrentRowReformatted() {
	return pvt->_currentrowreformatted;
}

char *sqlrservercursor::getErrorBuffer() {
	return pvt->_error;
}

uint32_t sqlrservercursor::getErrorLength() {
	return pvt->_errorlength;
}

void sqlrservercursor::setErrorLength(uint32_t errorlength) {
	pvt->_errorlength=errorlength;
}

uint32_t sqlrservercursor::getErrorNumber() {
	return pvt->_errnum;
}

void sqlrservercursor::setErrorNumber(uint32_t errnum) {
	pvt->_errnum=errnum;
}

bool sqlrservercursor::getLiveConnection() {
	return pvt->_liveconnection;
}

void sqlrservercursor::setLiveConnection(bool liveconnection) {
	pvt->_liveconnection=liveconnection;
}

void sqlrservercursor::setCreateTempTablePattern(const char *createtemp) {
	pvt->_createtemp.setPattern(createtemp);
	pvt->_createtemp.study();
}

const char *sqlrservercursor::skipCreateTempTableClause(const char *query) {
	const char	*ptr=conn->cont->skipWhitespaceAndComments(query);
	if (!charstring::isNullOrEmpty(ptr) && pvt->_createtemp.match(ptr)) {
		return pvt->_createtemp.getSubstringEnd(0);
	}
	return NULL;
}

bool sqlrservercursor::columnInfoIsValidAfterPrepare() {
	return false;
}

void sqlrservercursor::setColumnInfoIsValid(bool valid) {
	pvt->_columninfoisvalid=valid;
}

bool sqlrservercursor::getColumnInfoIsValid() {
	return pvt->_columninfoisvalid;
}

void sqlrservercursor::setQueryHasBeenPreProcessed(bool preprocessed) {
	pvt->_queryhasbeenpreprocessed=preprocessed;
}

bool sqlrservercursor::getQueryHasBeenPreProcessed() {
	return pvt->_queryhasbeenpreprocessed;
}

void sqlrservercursor::setQueryHasBeenPrepared(bool prepared) {
	pvt->_queryhasbeenprepared=prepared;
}

bool sqlrservercursor::getQueryHasBeenPrepared() {
	return pvt->_queryhasbeenprepared;
}

void sqlrservercursor::setQueryHasBeenExecuted(bool executed) {
	pvt->_queryhasbeenexecuted=executed;
}

bool sqlrservercursor::getQueryHasBeenExecuted() {
	return pvt->_queryhasbeenexecuted;
}

void sqlrservercursor::setQueryNeedsIntercept(bool intercept) {
	pvt->_queryneedsintercept=intercept;
}

bool sqlrservercursor::getQueryNeedsIntercept() {
	return pvt->_queryneedsintercept;
}

void sqlrservercursor::setQueryWasIntercepted(bool intercepted) {
	pvt->_querywasintercepted=intercepted;
}

bool sqlrservercursor::getQueryWasIntercepted() {
	return pvt->_querywasintercepted;
}

void sqlrservercursor::setBindsWereFaked(bool faked) {
	pvt->_bindswerefaked=faked;
}

bool sqlrservercursor::getBindsWereFaked() {
	return pvt->_bindswerefaked;
}

void sqlrservercursor::setFakeInputBindsForThisQuery(bool fake) {
	pvt->_fakeinputbindsforthisquery=fake;
}

bool sqlrservercursor::getFakeInputBindsForThisQuery() {
	return pvt->_fakeinputbindsforthisquery;
}

void sqlrservercursor::setQueryType(sqlrquerytype_t querytype) {
	pvt->_querytype=querytype;
}

sqlrquerytype_t sqlrservercursor::getQueryType() {
	return pvt->_querytype;
}

stringbuffer *sqlrservercursor::getQueryWithFakeInputBindsBuffer() {
	return &(pvt->_querywithfakeinputbinds);
}

void sqlrservercursor::allocateColumnPointers(uint32_t colcount) {
	pvt->_columnnames=new const char *[colcount];
	pvt->_columnnamelengths=new uint16_t[colcount];
	pvt->_columntypes=new uint16_t[colcount];
	pvt->_columntypenames=new const char *[colcount];
	pvt->_columntypenamelengths=new uint16_t[colcount];
	pvt->_columnlengths=new uint32_t[colcount];
	pvt->_columnprecisions=new uint32_t[colcount];
	pvt->_columnscales=new uint32_t[colcount];
	pvt->_columnisnullables=new uint16_t[colcount];
	pvt->_columnisprimarykeys=new uint16_t[colcount];
	pvt->_columnisuniques=new uint16_t[colcount];
	pvt->_columnispartofkeys=new uint16_t[colcount];
	pvt->_columnisunsigneds=new uint16_t[colcount];
	pvt->_columniszerofilleds=new uint16_t[colcount];
	pvt->_columnisbinarys=new uint16_t[colcount];
	pvt->_columnisautoincrements=new uint16_t[colcount];
	pvt->_columntables=new const char *[colcount];
	pvt->_columntablelengths=new uint16_t[colcount];
}

void sqlrservercursor::deallocateColumnPointers() {
	delete[] pvt->_columnnames;
	delete[] pvt->_columnnamelengths;
	delete[] pvt->_columntypes;
	delete[] pvt->_columntypenames;
	delete[] pvt->_columntypenamelengths;
	delete[] pvt->_columnlengths;
	delete[] pvt->_columnprecisions;
	delete[] pvt->_columnscales;
	delete[] pvt->_columnisnullables;
	delete[] pvt->_columnisprimarykeys;
	delete[] pvt->_columnisuniques;
	delete[] pvt->_columnispartofkeys;
	delete[] pvt->_columnisunsigneds;
	delete[] pvt->_columniszerofilleds;
	delete[] pvt->_columnisbinarys;
	delete[] pvt->_columnisautoincrements;
	delete[] pvt->_columntables;
	delete[] pvt->_columntablelengths;
}

void sqlrservercursor::getColumnPointers(const char ***columnnames,
					uint16_t **columnnamelengths,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamelengths,
					uint32_t **columnlengths,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablelengths) {

	// get the max column count
	uint32_t	colcount=conn->cont->getMaxColumnCount();

	// decide if we need to allocate field pointers here,
	// and if so, how many columns
	bool	allocate=false;
	if (!colcount) {
		colcount=colCount();
		allocate=true;
	}

	// allocate the field pointers, if necessary
	if (allocate) {
		deallocateColumnPointers();
		allocateColumnPointers(colcount);
	}

	// return the column pointers
	*columnnames=pvt->_columnnames;
	*columnnamelengths=pvt->_columnnamelengths;
	*columntypes=pvt->_columntypes;
	*columntypenames=pvt->_columntypenames;
	*columntypenamelengths=pvt->_columntypenamelengths;
	*columnlengths=pvt->_columnlengths;
	*columnprecisions=pvt->_columnprecisions;
	*columnscales=pvt->_columnscales;
	*columnisnullables=pvt->_columnisnullables;
	*columnisprimarykeys=pvt->_columnisprimarykeys;
	*columnisuniques=pvt->_columnisuniques;
	*columnispartofkeys=pvt->_columnispartofkeys;
	*columnisunsigneds=pvt->_columnisunsigneds;
	*columniszerofilleds=pvt->_columniszerofilleds;
	*columnisbinarys=pvt->_columnisbinarys;
	*columnisautoincrements=pvt->_columnisautoincrements;
	*columntables=pvt->_columntables;
	*columntablelengths=pvt->_columntablelengths;
}

const char *sqlrservercursor::getColumnNameFromBuffer(uint32_t col) {
	return pvt->_columnnames[col];
}

uint16_t sqlrservercursor::getColumnNameLengthFromBuffer(uint32_t col) {
	return pvt->_columnnamelengths[col];
}

uint16_t sqlrservercursor::getColumnTypeFromBuffer(uint32_t col) {
	return pvt->_columntypes[col];
}

const char *sqlrservercursor::getColumnTypeNameFromBuffer(uint32_t col) {
	return pvt->_columntypenames[col];
}

uint16_t sqlrservercursor::getColumnTypeNameLengthFromBuffer(uint32_t col) {
	return pvt->_columntypenamelengths[col];
}

uint32_t sqlrservercursor::getColumnLengthFromBuffer(uint32_t col) {
	return pvt->_columnlengths[col];
}

uint32_t sqlrservercursor::getColumnPrecisionFromBuffer(uint32_t col) {
	return pvt->_columnprecisions[col];
}

uint32_t sqlrservercursor::getColumnScaleFromBuffer(uint32_t col) {
	return pvt->_columnscales[col];
}

uint16_t sqlrservercursor::getColumnIsNullableFromBuffer(uint32_t col) {
	return pvt->_columnisnullables[col];
}

uint16_t sqlrservercursor::getColumnIsPrimaryKeyFromBuffer(uint32_t col) {
	return pvt->_columnisprimarykeys[col];
}

uint16_t sqlrservercursor::getColumnIsUniqueFromBuffer(uint32_t col) {
	return pvt->_columnisuniques[col];
}

uint16_t sqlrservercursor::getColumnIsPartOfKeyFromBuffer(uint32_t col) {
	return pvt->_columnispartofkeys[col];
}

uint16_t sqlrservercursor::getColumnIsUnsignedFromBuffer(uint32_t col) {
	return pvt->_columnisunsigneds[col];
}

uint16_t sqlrservercursor::getColumnIsZeroFilledFromBuffer(uint32_t col) {
	return pvt->_columniszerofilleds[col];
}

uint16_t sqlrservercursor::getColumnIsBinaryFromBuffer(uint32_t col) {
	return pvt->_columnisbinarys[col];
}

uint16_t sqlrservercursor::getColumnIsAutoIncrementFromBuffer(uint32_t col) {
	return pvt->_columnisautoincrements[col];
}

const char *sqlrservercursor::getColumnTableFromBuffer(uint32_t col) {
	return pvt->_columntables[col];
}

uint16_t sqlrservercursor::getColumnTableLengthFromBuffer(uint32_t col) {
	return pvt->_columntablelengths[col];
}

void sqlrservercursor::allocateFieldPointers(uint32_t colcount) {
	pvt->_fieldnames=new const char *[colcount];
	pvt->_fields=new const char *[colcount];
	pvt->_fieldlengths=new uint64_t[colcount];
	pvt->_blobs=new bool[colcount];
	pvt->_nulls=new bool[colcount];
}

void sqlrservercursor::deallocateFieldPointers() {
	delete[] pvt->_fieldnames;
	delete[] pvt->_fields;
	delete[] pvt->_fieldlengths;
	delete[] pvt->_blobs;
	delete[] pvt->_nulls;
}

void sqlrservercursor::getFieldPointers(const char ***fieldnames,
					const char ***fields,
					uint64_t **fieldlengths,
					bool **blobs,
					bool **nulls) {

	// get the max column count
	uint32_t	colcount=conn->cont->getMaxColumnCount();

	// decide if we need to allocate field pointers here,
	// and if so, how many columns
	bool	allocate=false;
	if (!colcount) {
		colcount=colCount();
		allocate=true;
	}

	// allocate the field pointers, if necessary
	if (allocate) {
		deallocateFieldPointers();
		allocateFieldPointers(colcount);
	}

	// return the field pointers
	*fieldnames=pvt->_fieldnames;
	*fields=pvt->_fields;
	*fieldlengths=pvt->_fieldlengths;
	*blobs=pvt->_blobs;
	*nulls=pvt->_nulls;
}

void sqlrservercursor::setQueryTimeout(uint64_t querytimeout) {
	pvt->_querytimeout=querytimeout;
}

uint64_t sqlrservercursor::getQueryTimeout() {
	return pvt->_querytimeout;
}

void sqlrservercursor::setExecuteDirect(bool executedirect) {
	pvt->_executedirect=executedirect;
}

bool sqlrservercursor::getExecuteDirect() {
	return pvt->_executedirect;
}

void sqlrservercursor::setExecuteRpc(bool executerpc) {
	pvt->_executerpc=executerpc;
}

bool sqlrservercursor::getExecuteRpc() {
	return pvt->_executerpc;
}

void sqlrservercursor::setResultSetHeaderHasBeenHandled(
				bool resultsetheaderhasbeenhandled) {
	pvt->_resultsetheaderhasbeenhandled=resultsetheaderhasbeenhandled;
}

bool sqlrservercursor::getResultSetHeaderHasBeenHandled() {
	return pvt->_resultsetheaderhasbeenhandled;
}

unsigned char *sqlrservercursor::getModuleData() {
	return pvt->_moduledata;
}
