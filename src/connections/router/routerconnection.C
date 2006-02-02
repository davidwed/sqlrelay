// Copyright (c) 2006 David Muse
// See the file COPYING for more information

#include <routerconnection.h>
#include <rudiments/rawbuffer.h>

#include <stdio.h>
#include <stdlib.h>

#include <datatypes.h>

routerconnection::routerconnection() : sqlrconnection_svr() {
	cons=NULL;
	concount=0;
	cfgfile=NULL;
	justloggedin=false;

	beginregex.compile("^\\s*begin");
	beginregex.study();
}

routerconnection::~routerconnection() {
	for (uint16_t index=0; index<concount; index++) {
		delete cons[index];
	}
	delete[] cons;
}

uint16_t routerconnection::getNumberOfConnectStringVars() {
	return 0;
}

bool routerconnection::supportsNativeBinds() {
	return true;
}

void routerconnection::handleConnectString() {

	cfgfile=cfgfl;

	linkedlist< routercontainer *>	*routerlist=cfgfl->getRouterList();
	concount=routerlist->getLength();

	cons=new sqlrconnection *[concount];
	for (uint16_t index=0; index<concount; index++) {
		routercontainer	*rn=routerlist->
					getNodeByIndex(index)->getData();
		cons[index]=new sqlrconnection(rn->getHost(),rn->getPort(),
						rn->getSocket(),rn->getUser(),
						rn->getPassword(),0,1);
	}
}

bool routerconnection::logIn() {
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
		bool	res=cons[index]->autoCommitOn();
		// FIXME: "do something" if this fails
		// The connection class calls autoCommitOn or autoCommitOff
		// immediately after logging in, which will cause the 
		// cons to connect to the relay's and tie them up unless we
		// call endSession.  We'd rather not tie them up until a
		// client connects, so if we just logged in, we'll call
		// endSession.
		if (justloggedin) {
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
		bool	res=cons[index]->autoCommitOff();
		// FIXME: "do something" if this fails
		// The connection class calls autoCommitOn or autoCommitOff
		// immediately after logging in, which will cause the 
		// cons to connect to the relay's and tie them up unless we
		// call endSession.  We'd rather not tie them up until a
		// client connects, so if we just logged in, we'll call
		// endSession.
		if (justloggedin) {
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
	// FIXME: wish I had 2 stage commit...
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		bool	res=cons[index]->commit();
		// FIXME: "do something" if this fails
		// Either commit or rollback will be called whenever the
		// client calls endSession() or disconnects.  We don't have a
		// way to tell if they just ran commit/rollback or actually
		// call endSession() or disconnected, but if the did call
		// endSession() or disconnect, we need to call endSession(),
		// so just to be safe, we'll do it.
		cons[index]->endSession();
		if (result) {
			result=res;
		}
	}
	return result;
}

bool routerconnection::rollback() {

	// commit all connections, if any fail, return failure
	// FIXME: wish I had 2 stage commit...
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		bool	res=cons[index]->rollback();
		// FIXME: "do something" if this fails
		// Either commit or rollback will be called whenever the
		// client calls endSession() or disconnects.  We don't have a
		// way to tell if they just ran commit/rollback or actually
		// call endSession() or disconnected, but if the did call
		// endSession() or disconnect, we need to call endSession(),
		// so just to be safe, we'll do it.
		cons[index]->endSession();
		if (result) {
			result=res;
		}
	}
	return result;
}

const char *routerconnection::identify() {
	return "router";
}

bool routerconnection::ping() {

	// ping all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		bool	res=cons[index]->ping();
		if (result) {
			result=res;
		}
	}
	return result;
}

routercursor::routercursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {
	routerconn=(routerconnection *)conn;
	nextrow=0;
	cur=NULL;
	curs=new sqlrcursor *[routerconn->concount];
	for (uint16_t index=0; index<routerconn->concount; index++) {
		curs[index]=new sqlrcursor(routerconn->cons[index]);
	}
	beginquery=false;
}

routercursor::~routercursor() {
	for (uint16_t index=0; index<routerconn->concount; index++) {
		delete curs[index];
	}
	delete[] curs;
}

bool routercursor::openCursor(uint16_t id) {
	return true;
}

bool routercursor::prepareQuery(const char *query, uint32_t length) {

	// check for a begin query
	beginquery=routerconn->beginregex.match(query);
	if (beginquery) {
		return true;
	}

	// look through the regular expressions and figure out which
	// connection this query needs to be run through
	uint16_t	conindex=0;
	routernode	*rcn=routerconn->cfgfile->
					getRouterList()->getNodeByIndex(0);
	while (rcn && !cur) {
		linkedlistnode< regularexpression * >	*ren=
					rcn->getData()->getRegexList()->
							getNodeByIndex(0);
		while (ren && !cur) {
			if (ren->getData()->match(query)) {
				cur=curs[conindex];
				cur->setResultSetBufferSize(FETCH_AT_ONCE);
			}
			ren=ren->getNext();
		}

		rcn=rcn->getNext();
		conindex++;
	}

	if (!cur) {
		return false;
	}

	// prepare the query using the cursor from whichever
	// connection turned out to be the right one
	cur->prepareQuery(query);

	return true;
}

bool routercursor::inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						int16_t *isnull) {
	cur->inputBind(variable,value);
	return true;
}

bool routercursor::inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value) {
	cur->inputBind(variable,*value);
	return true;
}

bool routercursor::inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale) {
	cur->inputBind(variable,*value,precision,scale);
	return true;
}

bool routercursor::inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
	cur->inputBindBlob(variable,value,valuesize);
	return true;
}

bool routercursor::inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
	cur->inputBindClob(variable,value,valuesize);
	return true;
}

// FIXME: output binds

bool routercursor::executeQuery(const char *query, uint32_t length,
							bool execute) {
	if (!execute) {
		return true;
	}
	if (beginquery) {
		return begin(query,length);
	}
	if (!cur) {
		if (!prepareQuery(query,length)) {
			return false;
		}
	}
	if (cur && cur->executeQuery()) {
		nextrow=0;
		return true;
	}
	return false;
}

bool routercursor::begin(const char *query, uint32_t length) {

	bool	result=true;
	for (uint16_t index=0; index<routerconn->concount; index++) {
		bool	res=curs[index]->sendQuery(query,length);
		// FIXME: "do something" if this fails
		if (result) {
			result=res;
			// if we had an error, set "cur" so
			// we can get the error from it
			if (!res && !cur) {
				cur=curs[index];
			}
		}
	}
	// If we're here with no "cur", then all the begin's must have
	// succeeded.  But we need a "cur" so everything else will work.
	// Any of them will do, so use the first one.
	if (!cur) {
		cur=curs[0];
	}
	return result;
}

const char *routercursor::errorMessage(bool *liveconnection) {
	// FIXME: detect downed database or downed relay
	*liveconnection=true;
	return cur->errorMessage();
}

bool routercursor::knowsRowCount() {
	return true;
}

uint64_t routercursor::rowCount() {
	return cur->rowCount();
}

bool routercursor::knowsAffectedRows() {
	return true;
}

uint64_t routercursor::affectedRows() {
	return cur->affectedRows();
}

uint32_t routercursor::colCount() {
	return cur->colCount();
}

const char * const * routercursor::columnNames() {
	return cur->getColumnNames();
}

uint16_t routercursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_NAMES;
}

void routercursor::returnColumnInfo() {
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
	return (cur->rowCount()==0);
}

bool routercursor::skipRow() {
	return fetchRow();
}

bool routercursor::fetchRow() {
	if (nextrow<cur->rowCount()) {
		nextrow++;
		return true;
	}
	return false;
}

void routercursor::returnRow() {
	for (uint32_t index=0; index<cur->colCount(); index++) {
		const char	*field=cur->getField(nextrow-1,index);
		if (!field) {
			conn->sendNullField();
		} else {
			conn->sendField(field,cur->getFieldLength(
							nextrow-1,index));
		}
	}
}

void routercursor::cleanUpData(bool freeresult, bool freebinds) {
	cur=NULL;
}
