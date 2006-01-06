// Copyright (c) 2006 David Muse
// See the file COPYING for more information

#include <routerconnection.h>
#include <rudiments/rawbuffer.h>

#include <stdio.h>
#include <stdlib.h>

#include <datatypes.h>

routerconnection::routerconnection() {
	cons=NULL;
	concount=0;
	cfgfile=cfgfl;
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

	routercontainer	**routerlist=cfgfl->getRouterList();
	concount=cfgfl->getRouterCount();

	cons=new sqlrconnection *[concount];
	for (uint16_t index=0; index<concount; index++) {
		cons[index]=new sqlrconnection(
					routerlist[index]->getHost(),
					routerlist[index]->getPort(),
					routerlist[index]->getSocket(),
					routerlist[index]->getUser(),
					routerlist[index]->getPassword(),0,1);
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
	currentrow=0;
	cur=NULL;
}

routercursor::~routercursor() {
	delete cur;
}

bool routercursor::openCursor(uint16_t id) {
	return true;
}

bool routercursor::prepareQuery(const char *query, uint32_t length) {

	for (uint16_t conindex=0; conindex<routerconn->concount; conindex++) {

		routercontainer	*rc=routerconn->cfgfile->
						getRouterList()[conindex];
		for (uint16_t regindex=0;
				regindex<rc->getRegexCount(); regindex++) {
			if (rc->getRegexList()[regindex]->match(query)) {
				cur=new sqlrcursor(routerconn->cons[conindex]);
				cur->setResultSetBufferSize(FETCH_AT_ONCE);
			}
		}
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
	if (execute) {
		if (cur->executeQuery()) {
			currentrow=0;
			return true;
		}
		return false;
	}
	return true;
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
	currentrow++;
	return true;
}

bool routercursor::fetchRow() {
	currentrow++;
	return true;
}

void routercursor::returnRow() {
	for (uint32_t index=0; index<cur->colCount(); index++) {
		const char	*field=cur->getField(currentrow,index);
		if (!field) {
			conn->sendNullField();
		} else {
			conn->sendField(field,cur->getFieldLength(
							currentrow,index));
		}
	}
}

void routercursor::cleanUpData(bool freeresult, bool freebinds) {
	if (freeresult) {
		delete cur;
		cur=NULL;
	}
}
