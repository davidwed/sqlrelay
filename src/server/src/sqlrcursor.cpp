// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrservercontroller.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <defines.h>

sqlrservercursor::sqlrservercursor(sqlrserverconnection *conn, uint16_t id) {

	this->conn=conn;

	maxerrorlength=conn->cont->cfgfl->getMaxErrorLength();

	setInputBindCount(0);
	inbindvars=new sqlrserverbindvar[conn->cont->cfgfl->getMaxBindCount()];
	setOutputBindCount(0);
	outbindvars=new sqlrserverbindvar[conn->cont->cfgfl->getMaxBindCount()];
	
	setState(SQLRCURSORSTATE_AVAILABLE);

	createtemp.compile("(create|CREATE|declare|DECLARE)[ 	\\r\\n]+((global|GLOBAL|local|LOCAL)?[ 	\\r\\n]+)?(temp|TEMP|temporary|TEMPORARY)?[ 	\\r\\n]+(table|TABLE)[ 	\\r\\n]+");

	querybuffer=new char[conn->cont->cfgfl->getMaxQuerySize()+1];
	setQueryLength(0);

	setQueryTree(NULL);

	error=new char[maxerrorlength+1];
	errorlength=0;
	errnum=0;
	liveconnection=true;

	setCommandStart(0,0);
	setCommandEnd(0,0);
	setQueryStart(0,0);
	setQueryEnd(0,0);

	setCustomQueryCursor(NULL);

	clearTotalRowsFetched();

	this->id=id;

	// sqlrservercontroller flags
	prepared=false;
	querywasintercepted=false;
	bindswerefaked=false;
	fakeinputbindsforthisquery=false;
}

sqlrservercursor::~sqlrservercursor() {
	delete[] querybuffer;
	delete querytree;
	delete[] inbindvars;
	delete[] outbindvars;
	delete customquerycursor;
	delete[] error;
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
				int32_t microsecond, const char *tz) {
	charstring::printf(buffer,buffersize,
				"%04d-%02d-%02d %02d:%02d:%02d",
				year,month,day,hour,minute,second);
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
					char *buffer,
					uint16_t buffersize,
					int16_t *isnull) {
	dateToString(buffer,buffersize,year,month,day,
			hour,minute,second,microsecond,tz);
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

	const char	*ptr=query;
	const char	*endptr=query+length;

	// skip any leading comments
	if (!conn->cont->skipWhitespace(&ptr,endptr) ||
		!conn->cont->skipComment(&ptr,endptr) ||
		!conn->cont->skipWhitespace(&ptr,endptr)) {
		return;
	}

	// see if the query matches the pattern for a temporary query that
	// creates a temporary table
	if (createtemp.match(ptr)) {
		ptr=createtemp.getSubstringEnd(0);
	} else {
		return;
	}

	// get the table name
	stringbuffer	tablename;
	while (ptr && *ptr && *ptr!=' ' &&
		*ptr!='\n' && *ptr!='	' && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	// append to list of temp tables
	conn->cont->addSessionTempTableForDrop(tablename.getString());
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
	const char	*ptr=conn->cont->skipWhitespaceAndComments(querybuffer);

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
	const char	*ptr=conn->cont->skipWhitespaceAndComments(querybuffer);

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
	return NULL;
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
	return id;
}

bool sqlrservercursor::fakeInputBinds() {

	// return false if there aren't any input binds
	if (!inbindcount) {
		return false;
	}

	// re-init the buffer
	querywithfakeinputbinds.clear();

	// loop through the query, performing substitutions
	char	prefix=inbindvars[0].variable[0];
	char	*ptr=querybuffer;
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
			for (int16_t i=0; i<inbindcount; i++) {

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
						inbindvars[i].variable+1)==
									index) 

					||

					(!charstring::compare(ptr,
						inbindvars[i].variable,
						inbindvars[i].variablesize) 
					 		&&
					(*(ptr+inbindvars[i].variablesize)==
					 		' ' ||
					*(ptr+inbindvars[i].variablesize)==
							'	' ||
					*(ptr+inbindvars[i].variablesize)==
							'\n' ||
					*(ptr+inbindvars[i].variablesize)==
							')' ||
					*(ptr+inbindvars[i].variablesize)==
							',' ||
					*(ptr+inbindvars[i].variablesize)==
							'\0')
					)) {

					performSubstitution(
						&querywithfakeinputbinds,i);
					if (*ptr=='?') {
						ptr++;
					} else {
						ptr=ptr+inbindvars[i].
								variablesize;
					}
					index++;
					break;
				}
			}
		}

		// write the input query to the output query
		if (*ptr) {
			querywithfakeinputbinds.append(*ptr);
			ptr++;
		}
	}
	return true;
}

void sqlrservercursor::performSubstitution(stringbuffer *buffer, int16_t index) {

	if (inbindvars[index].type==STRING_BIND ||
		inbindvars[index].type==CLOB_BIND) {

		buffer->append("'");

		size_t	length=inbindvars[index].valuesize;

		for (size_t ind=0; ind<length; ind++) {

			char	ch=inbindvars[index].value.stringval[ind];

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

	} else if (inbindvars[index].type==CLOB_BIND) {
		// FIXME: support BLOB_BINDs with an encodeBlob() method
	} else if (inbindvars[index].type==INTEGER_BIND) {
		buffer->append(inbindvars[index].value.integerval);
	} else if (inbindvars[index].type==DOUBLE_BIND) {
		char	*dbuf=NULL;
		if (!inbindvars[index].value.doubleval.precision &&
				!inbindvars[index].value.doubleval.scale) {
			dbuf=charstring::parseNumber(
				inbindvars[index].value.doubleval.value);
		} else {
			dbuf=charstring::parseNumber(
				inbindvars[index].value.doubleval.value,
				inbindvars[index].value.doubleval.precision,
				inbindvars[index].value.doubleval.scale);
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
	} else if (inbindvars[index].type==DATE_BIND) {
		char	buf[64];
		dateToString(buf,sizeof(buf),
				inbindvars[index].value.dateval.year,
				inbindvars[index].value.dateval.month,
				inbindvars[index].value.dateval.day,
				inbindvars[index].value.dateval.hour,
				inbindvars[index].value.dateval.minute,
				inbindvars[index].value.dateval.second,
				inbindvars[index].value.dateval.microsecond,
				inbindvars[index].value.dateval.tz);
		buffer->append("'")->append(buf)->append("'");
	} else if (inbindvars[index].type==NULL_BIND) {
		buffer->append("NULL");
	}
}

void sqlrservercursor::setInputBindCount(uint16_t inbindcount) {
	this->inbindcount=inbindcount;
}

uint16_t sqlrservercursor::getInputBindCount() {
	return inbindcount;
}

sqlrserverbindvar *sqlrservercursor::getInputBinds() {
	return inbindvars;
}

void sqlrservercursor::setOutputBindCount(uint16_t outbindcount) {
	this->outbindcount=outbindcount;
}

uint16_t sqlrservercursor::getOutputBindCount() {
	return outbindcount;
}

sqlrserverbindvar *sqlrservercursor::getOutputBinds() {
	return outbindvars;
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
	return querybuffer;
}

uint32_t sqlrservercursor::getQueryLength() {
	return querylength;
}

void sqlrservercursor::setQueryLength(uint32_t querylength) {
	this->querylength=querylength;
}

void sqlrservercursor::setQueryTree(xmldom *tree) {
	querytree=tree;
}

xmldom *sqlrservercursor::getQueryTree() {
	return querytree;
}

void sqlrservercursor::clearQueryTree() {
	delete querytree;
	querytree=NULL;
}

void sqlrservercursor::setCommandStart(uint64_t sec, uint64_t usec) {
	commandstartsec=sec;
	commandstartusec=usec;
}

uint64_t sqlrservercursor::getCommandStartSec() {
	return commandstartsec;
}

uint64_t sqlrservercursor::getCommandStartUSec() {
	return commandstartusec;
}


void sqlrservercursor::setCommandEnd(uint64_t sec, uint64_t usec) {
	commandendsec=sec;
	commandendusec=usec;
}

uint64_t sqlrservercursor::getCommandEndSec() {
	return commandendsec;
}

uint64_t sqlrservercursor::getCommandEndUSec() {
	return commandendusec;
}


void sqlrservercursor::setQueryStart(uint64_t sec, uint64_t usec) {
	querystartsec=sec;
	querystartusec=usec;
}

uint64_t sqlrservercursor::getQueryStartSec() {
	return querystartsec;
}

uint64_t sqlrservercursor::getQueryStartUSec() {
	return querystartusec;
}


void sqlrservercursor::setQueryEnd(uint64_t sec, uint64_t usec) {
	queryendsec=sec;
	queryendusec=usec;
}

uint64_t sqlrservercursor::getQueryEndSec() {
	return queryendsec;
}

uint64_t sqlrservercursor::getQueryEndUSec() {
	return queryendusec;
}

void sqlrservercursor::setState(sqlrcursorstate_t state) {
	this->state=state;
}

sqlrcursorstate_t sqlrservercursor::getState() {
	return state;
}

void sqlrservercursor::setCustomQueryCursor(sqlrquerycursor *cur) {
	customquerycursor=cur;
}

sqlrquerycursor *sqlrservercursor::getCustomQueryCursor() {
	return customquerycursor;
}

void sqlrservercursor::clearCustomQueryCursor() {
	delete customquerycursor;
	customquerycursor=NULL;
}

void sqlrservercursor::clearTotalRowsFetched() {
	totalrowsfetched=0;
}

uint64_t sqlrservercursor::getTotalRowsFetched() {
	return totalrowsfetched;
}

void sqlrservercursor::incrementTotalRowsFetched() {
	totalrowsfetched++;
}

void sqlrservercursor::clearError() {
	setError(NULL,0,true);
}

void sqlrservercursor::setError(const char *err, int64_t errn, bool liveconn) {
	errorlength=charstring::length(err);
	if (errorlength>maxerrorlength) {
		errorlength=maxerrorlength;
	}
	charstring::copy(error,err,errorlength);
	error[errorlength]='\0';
	errnum=errn;
	liveconnection=liveconn;
}

char *sqlrservercursor::getErrorBuffer() {
	return error;
}

uint32_t sqlrservercursor::getErrorLength() {
	return errorlength;
}

void sqlrservercursor::setErrorLength(uint32_t errorlength) {
	this->errorlength=errorlength;
}

uint32_t sqlrservercursor::getErrorNumber() {
	return errnum;
}

void sqlrservercursor::setErrorNumber(uint32_t errnum) {
	this->errnum=errnum;
}

bool sqlrservercursor::getLiveConnection() {
	return liveconnection;
}

void sqlrservercursor::setLiveConnection(bool liveconnection) {
	this->liveconnection=liveconnection;
}
