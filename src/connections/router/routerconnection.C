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

const char *routerconnection::identify() {
	return "router";
}

routercursor::routercursor(sqlrconnection_svr *conn) :
						sqlrcursor_svr(conn) {
	routerconn=(routerconnection *)conn;
	nextrow=0;
	cur=NULL;
}

routercursor::~routercursor() {
	delete cur;
}

bool routercursor::openCursor(uint16_t id) {
	return true;
}

bool routercursor::prepareQuery(const char *query, uint32_t length) {

	uint16_t	conindex=0;
	routernode	*rcn=routerconn->cfgfile->
					getRouterList()->getNodeByIndex(0);
	while (rcn && !cur) {
		linkedlistnode< regularexpression * >	*ren=
					rcn->getData()->getRegexList()->
							getNodeByIndex(0);
		while (ren && !cur) {
			if (ren->getData()->match(query)) {
				cur=new sqlrcursor(routerconn->cons[conindex]);
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

bool routercursor::executeQuery(const char *query, uint32_t length,
							bool execute) {
	if (!execute) {
		return true;
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

const char *routercursor::errorMessage(bool *liveconnection) {
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
	if (freeresult) {
		delete cur;
		cur=NULL;
	}
}
