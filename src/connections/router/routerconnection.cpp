// Copyright (c) 2006 David Muse
// See the file COPYING for more information

#include <routerconnection.h>
#include <rudiments/rawbuffer.h>
#include <rudiments/character.h>

#include <stdio.h>
#include <stdlib.h>

// debug getpid()
#include <unistd.h>

#include <datatypes.h>

routerconnection::routerconnection() : sqlrconnection_svr() {
	cons=NULL;
	cur=NULL;
	beginquery=NULL;
	anymustbegin=false;
	concount=0;
	cfgfile=NULL;
	justloggedin=false;
	nullbindvalue=nullBindValue();
	nonnullbindvalue=nonNullBindValue();

	beginregex.compile("^\\s*(begin|start\\s*transaction)");
	beginregex.study();
	beginendregex.compile("^\\s*begin\\s*.*\\s*end;\\s*$");
	beginendregex.study();
}

routerconnection::~routerconnection() {
	for (uint16_t index=0; index<concount; index++) {
		delete cons[index];
	}
	delete[] cons;
	delete[] beginquery;
}

bool routerconnection::supportsAuthOnDatabase() {
	return false;
}

uint16_t routerconnection::getNumberOfConnectStringVars() {
	return 0;
}

void routerconnection::handleConnectString() {

	cfgfile=cfgfl;

	linkedlist< routecontainer * >	*routelist=cfgfl->getRouteList();
	concount=routelist->getLength();

	cons=new sqlrconnection *[concount];
	beginquery=new const char *[concount];
	anymustbegin=false;
	for (uint16_t index=0; index<concount; index++) {

		cons[index]=NULL;
		beginquery[index]=NULL;

		routecontainer	*rn=routelist->
					getNodeByIndex(index)->getData();

		// empty host/port/socket/user/password means that queries
		// going to this connection will be filtered out
		if (rn->getIsFilter()) {
			continue;
		}

		cons[index]=new sqlrconnection(rn->getHost(),rn->getPort(),
						rn->getSocket(),rn->getUser(),
						rn->getPassword(),0,1);

		const char	*id=cons[index]->identify();
		if (!charstring::compare(id,"sybase") ||
				!charstring::compare(id,"freetds")) {
			beginquery[index]="begin tran";
		} else if (!charstring::compare(id,"sqlite")) {
			beginquery[index]="begin transaction";
		} else if (!charstring::compare(id,"postgresql") ||
				!charstring::compare(id,"router")) {
			beginquery[index]="begin";
		}

		if (beginquery[index]) {
			anymustbegin=true;
		}
	}
}

bool routerconnection::logIn(bool printerrors) {
	justloggedin=true;
	return true;
}

sqlrcursor_svr *routerconnection::initCursor() {
	return (sqlrcursor_svr *)new
			routercursor((sqlrconnection_svr *)this);
}

void routerconnection::deleteCursor(sqlrcursor_svr *curs) {
	delete (routercursor *)curs;
}

void routerconnection::logOut() {
}

bool routerconnection::autoCommitOn() {

	// turn autocommit on for all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->autoCommitOn();
		if (!res) {
			autoCommitOnFailed(index);
		}
		// The connection class calls autoCommitOn or autoCommitOff
		// immediately after logging in, which will cause the 
		// cons to connect to the relay's and tie them up unless we
		// call endSession.  We'd rather not tie them up until a
		// client connects, so if we just logged in, we'll call
		// endSession.
		if (justloggedin) {
			// if any of the connections must begin transactions,
			// then those connections will start off in auto-commit
			// mode no matter what, so put all connections in
			// autocommit mode
			// (this is a convenient place to do this...)
			if (anymustbegin) {
				cons[index]->autoCommitOn();
			}
			cons[index]->endSession();
		}
		if (result) {
			result=res;
		}
	}
	if (justloggedin) {
		justloggedin=false;
	}
	return result;
}

bool routerconnection::autoCommitOff() {

	// turn autocommit on for all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->autoCommitOff();
		if (!res) {
			autoCommitOffFailed(index);
		}
		// The connection class calls autoCommitOn or autoCommitOff
		// immediately after logging in, which will cause the 
		// cons to connect to the relay's and tie them up unless we
		// call endSession.  We'd rather not tie them up until a
		// client connects, so if we just logged in, we'll call
		// endSession.
		if (justloggedin) {
			// if any of the connections must begin transactions,
			// then those connections will start off in auto-commit
			// mode no matter what, so put all connections in
			// autocommit mode, even if autocommit-off is called
			// here
			// (this is a convenient place to do this...)
			if (anymustbegin) {
				cons[index]->autoCommitOn();
			}
			cons[index]->endSession();
		}
		if (result) {
			result=res;
		}
	}
	if (justloggedin) {
		justloggedin=false;
	}
	return result;
}

bool routerconnection::commit() {

	// commit all connections, if any fail, return failure
	// FIXME: use 2 stage commit...
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->commit();
		if (!res) {
			commitFailed(index);
		}
		if (result) {
			result=res;
		}
	}
	return result;
}

bool routerconnection::rollback() {

	// commit all connections, if any fail, return failure
	// FIXME: use 2 stage commit...
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->rollback();
		if (!res) {
			rollbackFailed(index);
		}
		if (result) {
			result=res;
		}
	}
	return result;
}

void routerconnection::endSession() {
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		cons[index]->endSession();
	}
}

const char *routerconnection::identify() {
	return "router";
}

const char *routerconnection::dbVersion() {
	// FIXME: return SQL Relay version?
	return "";
}

bool routerconnection::ping() {

	// ping all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->ping();
		if (result) {
			result=res;
		}
	}
	return result;
}

bool routerconnection::getLastInsertId(uint64_t *id, char **error) {

	// run it against the previously used connection,
	// unless there wasn't one
	if (!cur) {
		*id=0;
		return true;
	}
	*id=cur->getLastInsertId();
	*error=NULL;
	if (*id==0) {
		*error=charstring::duplicate(cur->errorMessage());
		if (*error) {
			return false;
		}
	}
	return true;
}

routercursor::routercursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {
	routerconn=(routerconnection *)conn;
	nextrow=0;
	con=NULL;
	cur=NULL;
	isbindcur=false;
	curindex=0;
	curs=new sqlrcursor *[routerconn->concount];
	for (uint16_t index=0; index<routerconn->concount; index++) {
		curs[index]=NULL;
		if (!routerconn->cons[index]) {
			continue;
		}
		curs[index]=new sqlrcursor(routerconn->cons[index]);
		curs[index]->setResultSetBufferSize(FETCH_AT_ONCE);
	}
	beginquery=false;
	obcount=0;
	cbcount=0;

	createoratemp.compile("(create|CREATE)[ \\t\\n\\r]+(global|GLOBAL)[ \\t\\n\\r]+(temporary|TEMPORARY)[ \\t\\n\\r]+(table|TABLE)[ \\t\\n\\r]+");
	preserverows.compile("(on|ON)[ \\t\\n\\r]+(commit|COMMIT)[ \\t\\n\\r]+(preserve|PRESERVE)[ \\t\\n\\r]+(rows|ROWS)");
}

routercursor::~routercursor() {
	for (uint16_t index=0; index<routerconn->concount; index++) {
		delete curs[index];
	}
	delete[] curs;
}

bool routercursor::prepareQuery(const char *query, uint32_t length) {

	// convert to lowercase and normalize whitespace, for regex matching
	char	*nquery=new char[length+1];
	if (nquery && query) {
		for (uint32_t i=0; i<length; ++i) {
			char	c=query[i];
			if (character::isWhitespace(c)) {
				nquery[i]=' ';
			} else {
				nquery[i]=character::toLowerCase(c);
			}
		}
		nquery[length]='\0';
	}

	// check for a begin query, but not "begin ... end;" query
	beginquery=(routerconn->beginregex.match(nquery) &&
			!routerconn->beginendregex.match(nquery));
	if (beginquery) {
		if (nquery) {
			delete[] nquery;
		}
		return true;
	}

	// reset cur and pointers
	con=NULL;
	if (isbindcur) {
		delete cur;
	}
	cur=NULL;
	isbindcur=false;
	curindex=0;

	// initialize the output bind count
	obcount=0;

	// initialize the cursor bind count
	cbcount=0;

	// look through the regular expressions and figure out which
	// connection this query needs to be run through
	uint16_t	conindex=0;
	routenode	*rcn=routerconn->cfgfile->
					getRouteList()->getFirstNode();
	bool	found=false;
	while (rcn && !found) {
		linkedlistnode< regularexpression * >	*ren=
					rcn->getData()->getRegexList()->
							getFirstNode();

		while (ren && !found) {
			if (ren->getData()->match(nquery)) {
				con=routerconn->cons[conindex];
				routerconn->cur=con;
				cur=curs[conindex];
				curindex=conindex;
				found=true;
			}
			ren=ren->getNext();
		}

		if (!found) {
			rcn=rcn->getNext();
			conindex++;
		}
	}

	// free normalized query
	if (nquery) {
		delete[] nquery;
	}

	// cur could be NULL here either because no connection could be found
	// to run the query, or because the query matched the pattern of
	// a connection which was intentionally set up to filter out the query
	if (!cur) {
		return false;
	}

	// prepare the query using the cursor from whichever
	// connection turned out to be the right one
	cur->prepareQuery(query);

	return true;
}

bool routercursor::supportsNativeBinds() {
	return true;
}

bool routercursor::inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
	cur->inputBind(variable+1,value);
	return true;
}

bool routercursor::inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value) {
	cur->inputBind(variable+1,*value);
	return true;
}

bool routercursor::inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale) {
	cur->inputBind(variable+1,*value,precision,scale);
	return true;
}

bool routercursor::inputBindDate(const char *variable,
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
	cur->inputBind(variable+1,year,month,day,
			hour,minute,second,microsecond,tz);
	return true;
}

bool routercursor::inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
	cur->inputBindBlob(variable+1,value,valuesize);
	return true;
}

bool routercursor::inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
	cur->inputBindClob(variable+1,value,valuesize);
	return true;
}

bool routercursor::outputBindString(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint16_t valuesize,
						int16_t *isnull) {
	cur->defineOutputBindString(variable+1,valuesize);
	obv[obcount].variable=variable+1;
	obv[obcount].type=STRING_BIND;
	obv[obcount].value.stringvalue=value;
	obv[obcount].valuesize=valuesize;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull) {
	cur->defineOutputBindInteger(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=INTEGER_BIND;
	obv[obcount].value.intvalue=value;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull) {
	cur->defineOutputBindDouble(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=DOUBLE_BIND;
	obv[obcount].value.doublevalue=value;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBindDate(const char *variable,
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
	cur->defineOutputBindDouble(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=DATE_BIND;
	obv[obcount].value.datevalue.year=year;
	obv[obcount].value.datevalue.month=month;
	obv[obcount].value.datevalue.day=day;
	obv[obcount].value.datevalue.hour=hour;
	obv[obcount].value.datevalue.minute=minute;
	obv[obcount].value.datevalue.second=second;
	obv[obcount].value.datevalue.microsecond=microsecond;
	obv[obcount].value.datevalue.tz=tz;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}


bool routercursor::outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull) {
	cur->defineOutputBindBlob(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=BLOB_BIND;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull) {
	cur->defineOutputBindClob(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=CLOB_BIND;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrcursor_svr *cursor) {
	cur->defineOutputBindCursor(variable+1);
	cbv[cbcount].variable=variable+1;
	cbv[cbcount].cursor=cursor;
	cbcount++;
	return true;
}

void routercursor::returnOutputBindBlob(uint16_t index) {
	const char	*varname=obv[index].variable;
	uint32_t	length=cur->getOutputBindLength(varname);
	conn->startSendingLong(length);
	conn->sendLongSegment(cur->getOutputBindBlob(varname),length);
	conn->endSendingLong();
}

void routercursor::returnOutputBindClob(uint16_t index) {
	const char	*varname=obv[index].variable;
	uint32_t	length=cur->getOutputBindLength(varname);
	conn->startSendingLong(length);
	conn->sendLongSegment(cur->getOutputBindClob(varname),length);
	conn->endSendingLong();
}

bool routercursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	if (!execute) {
		return true;
	}

	if (beginquery) {
		if (routerconn->anymustbegin) {
			return begin(query,length);
		} else {
			nextrow=0;
			return true;
		}
	}

	if (!cur) {
		if (!prepareQuery(query,length)) {
			return false;
		}
	}

	if (!cur || !cur->executeQuery()) {
		return false;
	}

	checkForTempTable(query,length);

	nextrow=0;

	// populate output bind values
	for (uint16_t index=0; index<obcount; index++) {
		const char	*variable=obv[index].variable;
		*(obv[index].isnull)=routerconn->nonnullbindvalue;
		if (obv[index].type==STRING_BIND) {
			const char	*str=cur->getOutputBindString(variable);
			uint32_t	len=cur->getOutputBindLength(variable);
			if (str) {
				charstring::copy(obv[index].value.stringvalue,
								str,len);
			} else {
				obv[index].value.stringvalue[0]='\0';
				*(obv[index].isnull)=routerconn->nullbindvalue;
			} 
		} else if (obv[index].type==INTEGER_BIND) {
			*(obv[index].value.intvalue)=
					cur->getOutputBindInteger(variable);
		} else if (obv[index].type==DOUBLE_BIND) {
			*(obv[index].value.doublevalue)=
					cur->getOutputBindDouble(variable);
		} else if (obv[index].type==DATE_BIND) {
			cur->getOutputBindDate(variable,
					obv[index].value.datevalue.year,
					obv[index].value.datevalue.month,
					obv[index].value.datevalue.day,
					obv[index].value.datevalue.hour,
					obv[index].value.datevalue.minute,
					obv[index].value.datevalue.second,
					obv[index].value.datevalue.microsecond,
					obv[index].value.datevalue.tz);
		}
	}

	// handle cursor bind values
	for (uint16_t index=0; index<cbcount; index++) {
		routercursor	*rcur=(routercursor *)cbv[index].cursor;
		rcur->con=con;
		rcur->cur=cur->getOutputBindCursor(cbv[index].variable);
		if (!rcur->cur) {
			return false;
		}
		rcur->cur->setResultSetBufferSize(FETCH_AT_ONCE);
		rcur->isbindcur=true;
		rcur->nextrow=0;
		if (!rcur->cur->fetchFromBindCursor()) {
			return false;
		}
	}
	return true;
}


void routercursor::checkForTempTable(const char *query, uint32_t length) {

	// for non-oracle db's
	if (charstring::compare(con->identify(),"oracle8")) {
		sqlrcursor_svr::checkForTempTable(query,length);
		return;
	}

	// for oracle db's...

	char	*ptr=(char *)query;
	char	*endptr=(char *)query+length;

	// skip any leading comments
	if (!skipWhitespace(&ptr,endptr) || !skipComment(&ptr,endptr) ||
		!skipWhitespace(&ptr,endptr)) {
		return;
	}

	// look for "create global temporary table "
	if (createoratemp.match(ptr)) {
		ptr=createoratemp.getSubstringEnd(0);
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
	// check for "on commit preserve rows" otherwise assume
	// "on commit delete rows"
	if (preserverows.match(ptr)) {
		conn->addSessionTempTableForTrunc(tablename.getString());
	}
}

bool routercursor::begin(const char *query, uint32_t length) {

	bool	result=true;
	for (uint16_t index=0; index<routerconn->concount; index++) {

		if (!routerconn->cons[index]) {
			continue;
		}

		// for databases that allow begin's, run the begin query,
		// for others, just set autocommit off
		bool	res=false;
		if (routerconn->beginquery[index]) {
			res=curs[index]->sendQuery(
						routerconn->beginquery[index],
						length);
			if (!res) {
				routerconn->beginQueryFailed(index);
			}
		} else {
			res=routerconn->cons[index]->autoCommitOff();
			if (!res) {
				routerconn->autoCommitOffFailed(index);
			}
		}
		if (result) {
			result=res;
			// if we had an error, set "cur" so
			// we can get the error from it
			if (!res && !cur) {
				cur=curs[index];
				curindex=0;
			}
		}
	}
	// If we're here with no "cur", then all the begin's must have
	// succeeded.  But we need a "cur" so everything else will work.
	// Any of them will do, so use the first one.
	/*if (!cur) {
		cur=curs[0];
		curindex=0;
	}*/
	return result;
}

void routercursor::errorMessage(const char **errormessage,
				int64_t *errorcode,
				bool *liveconnection) {
	// FIXME: detect downed database or downed relay
	*liveconnection=true;
	*errormessage=(cur)?cur->errorMessage():"";
	*errorcode=cur->errorNumber();
	*errorcode=0;
}

bool routercursor::knowsRowCount() {
	return true;
}

uint64_t routercursor::rowCount() {
	return (cur)?cur->rowCount():0;
}

bool routercursor::knowsAffectedRows() {
	return true;
}

uint64_t routercursor::affectedRows() {
	return (cur)?cur->affectedRows():0;
}

uint32_t routercursor::colCount() {
	return (cur)?cur->colCount():0;
}

const char * const * routercursor::columnNames() {
	return (cur)?cur->getColumnNames():0;
}

uint16_t routercursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_NAMES;
}

void routercursor::returnColumnInfo() {
	if (!cur) {
		return;
	}
	for (uint32_t index=0; index<cur->colCount(); index++) {
		const char	*name=cur->getColumnName(index);
		const char	*typestring=cur->getColumnType(index);
		conn->sendColumnDefinitionString(name,
					charstring::length(name),
					typestring,
					charstring::length(typestring),
					cur->getColumnLength(index),
					cur->getColumnPrecision(index),
					cur->getColumnScale(index),
					cur->getColumnIsNullable(index),
					cur->getColumnIsPrimaryKey(index),
					cur->getColumnIsUnique(index),
					cur->getColumnIsPartOfKey(index),
					cur->getColumnIsUnsigned(index),
					cur->getColumnIsZeroFilled(index),
					cur->getColumnIsBinary(index),
					cur->getColumnIsAutoIncrement(index));
	}
}

bool routercursor::noRowsToReturn() {
	return (((cur)?cur->rowCount():0)==0);
}

bool routercursor::skipRow() {
	return fetchRow();
}

bool routercursor::fetchRow() {
	if (!cur) {
		return false;
	}
	if (cur->getField(nextrow,(uint32_t)0)) {
		nextrow++;
		return true;
	}
	return false;
}

void routercursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {
	const char	*fld=cur->getField(nextrow-1,col);
	uint32_t	len=cur->getFieldLength(nextrow-1,col);
	if (len) {
		*field=fld;
		*fieldlength=len;
	} else {
		*null=true;
	}
}

void routercursor::cleanUpData(bool freeresult, bool freebinds) {
	if (freebinds) {
		if (cur) {
			cur->clearBinds();
		}
		obcount=0;
		cbcount=0;
	}
}

// FIXME: "do something" when these failures occur
void routerconnection::autoCommitOnFailed(uint16_t index) {
}

void routerconnection::autoCommitOffFailed(uint16_t index) {
}

void routerconnection::commitFailed(uint16_t index) {
}

void routerconnection::rollbackFailed(uint16_t index) {
}

void routerconnection::beginQueryFailed(uint16_t index) {
}
