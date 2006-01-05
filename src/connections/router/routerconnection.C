// Copyright (c) 2006 David Muse
// See the file COPYING for more information

#include <routerconnection.h>
#include <rudiments/rawbuffer.h>

#include <stdio.h>
#include <stdlib.h>

#include <datatypes.h>

routerconnection::routerconnection() {
}

routerconnection::~routerconnection() {
}

uint16_t routerconnection::getNumberOfConnectStringVars() {
	return 0;
}

bool routerconnection::supportsNativeBinds() {
	return true;
}

void routerconnection::handleConnectString() {
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
}

routercursor::~routercursor() {
}

bool routercursor::openCursor(uint16_t id) {
	return true;
}

bool routercursor::prepareQuery(const char *query, uint32_t length) {
	return true;
}

bool routercursor::inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						int16_t *isnull) {
	return true;
}

bool routercursor::inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value) {
	return true;
}

bool routercursor::inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale) {
	return true;
}

bool routercursor::inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
	return true;
}

bool routercursor::inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull) {
	return true;
}

bool routercursor::executeQuery(const char *query, uint32_t length,
							bool execute) {
	return true;
}

const char *routercursor::errorMessage(bool *liveconnection) {
	return "";
}

bool routercursor::knowsRowCount() {
	return true;
}

uint64_t routercursor::rowCount() {
	return 0;
}

bool routercursor::knowsAffectedRows() {
	return true;
}

uint64_t routercursor::affectedRows() {
	return 0;
}

uint32_t routercursor::colCount() {
	return 0;
}

const char * const * routercursor::columnNames() {
	return NULL;
}

uint16_t routercursor::columnTypeFormat() {
	return 0;
}

void routercursor::returnColumnInfo() {
	for (;;) {
		conn->sendColumnDefinitionString(name,
						charstring::length(name),
						typestring,
						charstring::length(typestring),
						size,
						0,0,0,0,0,
						0,0,0,binary,0);
	}
}

bool routercursor::noRowsToReturn() {
	return true;
}

bool routercursor::skipRow() {
	return true;
}

bool routercursor::fetchRow() {
	return true;
}

void routercursor::returnRow() {
}

void routercursor::cleanUpData(bool freeresult, bool freebinds) {
}
