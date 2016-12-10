// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#define NEED_DATATYPESTRING 1
#include <datatypes.h>
#include <defines.h>

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

		uint16_t		_inbindcount;
		sqlrserverbindvar	*_inbindvars;
		uint16_t		_outbindcount;
		sqlrserverbindvar	*_outbindvars;

		uint64_t	_totalrowsfetched;

		uint64_t	_commandstartsec;
		uint64_t	_commandstartusec;
		uint64_t	_commandendsec;
		uint64_t	_commandendusec;
		uint64_t	_querystartsec;
		uint64_t	_querystartusec;
		uint64_t	_queryendsec;
		uint64_t	_queryendusec;

		uint32_t	_maxerrorlength;

		char		*_error;
		uint32_t	_errorlength;
		int64_t		_errnum;
		bool		_liveconnection;
		bool		_errorsetmanually;

		sqlrcursorstate_t	_state;

		sqlrquerycursor		*_customquerycursor;

		bool	_queryhasbeenprepared;
		bool	_querywasintercepted;
		bool	_bindswerefaked;
		bool	_fakeinputbindsforthisquery;
};

sqlrservercursor::sqlrservercursor(sqlrserverconnection *conn, uint16_t id) {

	pvt=new sqlrservercursorprivate;

	this->conn=conn;

	pvt->_maxerrorlength=conn->cont->getConfig()->getMaxErrorLength();

	setInputBindCount(0);
	pvt->_inbindvars=new sqlrserverbindvar[
				conn->cont->getConfig()->getMaxBindCount()];
	setOutputBindCount(0);
	pvt->_outbindvars=new sqlrserverbindvar[
				conn->cont->getConfig()->getMaxBindCount()];
	
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
	pvt->_errorsetmanually=false;

	setCommandStart(0,0);
	setCommandEnd(0,0);
	setQueryStart(0,0);
	setQueryEnd(0,0);

	setCustomQueryCursor(NULL);

	clearTotalRowsFetched();

	pvt->_id=id;

	pvt->_queryhasbeenprepared=false;
	pvt->_querywasintercepted=false;
	pvt->_bindswerefaked=false;
	pvt->_fakeinputbindsforthisquery=false;
}

sqlrservercursor::~sqlrservercursor() {
	delete[] pvt->_querybuffer;
	delete pvt->_querytree;
	delete[] pvt->_inbindvars;
	delete[] pvt->_outbindvars;
	delete pvt->_customquerycursor;
	delete[] pvt->_error;
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
	}

	// verify that there's whitespace after the query type
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
					uint16_t valuesize, 
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

bool sqlrservercursor::ignoreDateDdMmParameter(uint32_t col,
					const char *data, uint32_t size) {
	return false;
}

bool sqlrservercursor::noRowsToReturn() {
	return true;
}

bool sqlrservercursor::skipRow() {
	return fetchRow();
}

bool sqlrservercursor::fetchRow() {
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

bool sqlrservercursor::getColumnNameList(stringbuffer *output) {
	for (uint32_t i=0; i<colCount(); i++) {
		if (i) {
			output->append(',');
		}
		output->append(getColumnName(i),getColumnNameLength(i));
	}
	return true;
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
	char	prefix=pvt->_inbindvars[0].variable[0];
	char	*ptr=pvt->_querybuffer;
	int	index=1;
	bool	inquotes=false;
	while (*ptr) {

		// are we inside of quotes ?
		if (*ptr=='\'') {
			if (inquotes) {
				inquotes=false;
			} else {
				inquotes=true;
			}
		}

		// look for the bind var prefix or ? if not inside of quotes
		if (!inquotes && (*ptr==prefix || *ptr=='?')) {

			// look through the list of vars
			for (int16_t i=0; i<pvt->_inbindcount; i++) {

				// if we find a match, perform the substitution
				// and skip past the variable...
				//
				// for bind-by-name cases, we need to check
				// after the variable for whitespace, a comma
				// or a right parenthesis to make sure that we 
				// don't make the following mistake:
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
							variable+1)==index) 

					||

					(!charstring::compare(ptr,
						pvt->_inbindvars[i].
							variable,
						pvt->_inbindvars[i].
							variablesize) 
					 		&&
					(*(ptr+pvt->_inbindvars[i].
						variablesize)==' ' ||
					*(ptr+pvt->_inbindvars[i].
						variablesize)=='	' ||
					*(ptr+pvt->_inbindvars[i].
						variablesize)=='\n' ||
					*(ptr+pvt->_inbindvars[i].
						variablesize)==')' ||
					*(ptr+pvt->_inbindvars[i].
						variablesize)==',' ||
					*(ptr+pvt->_inbindvars[i].
						variablesize)=='\0')
					)) {

					performSubstitution(
					&(pvt->_querywithfakeinputbinds),i);
					if (*ptr=='?') {
						ptr++;
					} else {
						ptr=ptr+pvt->_inbindvars[i].
								variablesize;
					}
					index++;
					break;
				}
			}
		}

		// write the input query to the output query
		if (*ptr) {
			pvt->_querywithfakeinputbinds.append(*ptr);
			ptr++;
		}
	}
	return true;
}

void sqlrservercursor::performSubstitution(stringbuffer *buffer,
							int16_t index) {

	if (pvt->_inbindvars[index].type==SQLRSERVERBINDVARTYPE_STRING ||
		pvt->_inbindvars[index].type==SQLRSERVERBINDVARTYPE_CLOB) {

		buffer->append("'");

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

		buffer->append("'");

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

void sqlrservercursor::clearError() {
	setError(NULL,0,true);
	pvt->_errorsetmanually=false;
}

void sqlrservercursor::setError(const char *err, int64_t errn, bool liveconn) {
	pvt->_errorlength=charstring::length(err);
	if (pvt->_errorlength>pvt->_maxerrorlength) {
		pvt->_errorlength=pvt->_maxerrorlength;
	}
	charstring::copy(pvt->_error,err,pvt->_errorlength);
	pvt->_error[pvt->_errorlength]='\0';
	pvt->_errnum=errn;
	pvt->_liveconnection=liveconn;
	pvt->_errorsetmanually=true;
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

bool sqlrservercursor::getErrorSetManually() {
	return pvt->_errorsetmanually;
}

void sqlrservercursor::setErrorSetManually(bool errorsetmanually) {
	pvt->_errorsetmanually=errorsetmanually;
}

void sqlrservercursor::setCreateTempTablePattern(const char *createtemp) {
	pvt->_createtemp.compile(createtemp);
	pvt->_createtemp.study();
}

const char *sqlrservercursor::skipCreateTempTableClause(const char *query) {
	const char	*ptr=conn->cont->skipWhitespaceAndComments(query);
	if (!charstring::isNullOrEmpty(ptr) && pvt->_createtemp.match(ptr)) {
		return pvt->_createtemp.getSubstringEnd(0);
	}
	return NULL;
}

void sqlrservercursor::setQueryHasBeenPrepared(bool queryhasbeenprepared) {
	pvt->_queryhasbeenprepared=queryhasbeenprepared;
}

bool sqlrservercursor::getQueryHasBeenPrepared() {
	return pvt->_queryhasbeenprepared;
}

void sqlrservercursor::setQueryWasIntercepted(bool querywasintercepted) {
	pvt->_querywasintercepted=querywasintercepted;
}

bool sqlrservercursor::getQueryWasIntercepted() {
	return pvt->_querywasintercepted;
}

void sqlrservercursor::setBindsWereFaked(bool bindswerefaked) {
	pvt->_bindswerefaked=bindswerefaked;
}

bool sqlrservercursor::getBindsWereFaked() {
	return pvt->_bindswerefaked;
}

void sqlrservercursor::setFakeInputBindsForThisQuery(
					bool fakeinputbindsforthisquery) {
	pvt->_fakeinputbindsforthisquery=fakeinputbindsforthisquery;
}

bool sqlrservercursor::getFakeInputBindsForThisQuery() {
	return pvt->_fakeinputbindsforthisquery;
}

stringbuffer *sqlrservercursor::getQueryWithFakeInputBindsBuffer() {
	return &(pvt->_querywithfakeinputbinds);
}
