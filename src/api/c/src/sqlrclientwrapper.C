/* Copyright (c) 2000-2001  David Muse
   See the file COPYING for more information */

#include <sqlrelay/sqlrclient.h>

extern "C" {

#include <sqlrelay/sqlrclientwrapper.h>

sqlrcon sqlrcon_alloc(const char *server, uint16_t port, const char *socket,
				const char *user, const char *password, 
				int32_t retrytime, int32_t tries) {
	sqlrcon	sqlrconref=new sqlrconnection(server,port,socket,
					user,password,retrytime,tries);
	return sqlrconref;
}

void sqlrcon_free(sqlrcon sqlrconref) {
	delete (sqlrconnection *)sqlrconref;
}

void sqlrcon_endSession(sqlrcon sqlrconref) {
	sqlrconref->endSession();
}

int sqlrcon_suspendSession(sqlrcon sqlrconref) {
	return sqlrconref->suspendSession();
}

uint16_t sqlrcon_getConnectionPort(sqlrcon sqlrconref) {
	return sqlrconref->getConnectionPort();
}

const char *sqlrcon_getConnectionSocket(sqlrcon sqlrconref) {
	return sqlrconref->getConnectionSocket();
}

int sqlrcon_resumeSession(sqlrcon sqlrconref, uint16_t port,
					const char *socket) {
	return sqlrconref->resumeSession(port,socket);
}

int sqlrcon_ping(sqlrcon sqlrconref) {
	return sqlrconref->ping();
}

const char *sqlrcon_identify(sqlrcon sqlrconref) {
	return sqlrconref->identify();
}

const char *sqlrcon_dbVersion(sqlrcon sqlrconref) {
	return sqlrconref->dbVersion();
}

const char *sqlrcon_bindFormat(sqlrcon sqlrconref) {
	return sqlrconref->bindFormat();
}

int sqlrcon_autoCommitOn(sqlrcon sqlrconref) {
	return sqlrconref->autoCommitOn();
}

int sqlrcon_autoCommitOff(sqlrcon sqlrconref) {
	return sqlrconref->autoCommitOff();
}

int sqlrcon_commit(sqlrcon sqlrconref) {
	return sqlrconref->commit();
}

int sqlrcon_rollback(sqlrcon sqlrconref) {
	return sqlrconref->rollback();
}

void sqlrcon_debugOn(sqlrcon sqlrconref) {
	sqlrconref->debugOn();
}

void sqlrcon_debugOff(sqlrcon sqlrconref) {
	sqlrconref->debugOff();
}

int sqlrcon_getDebug(sqlrcon sqlrconref) {
	return sqlrconref->getDebug();
}

void sqlrcon_debugPrintFunction(sqlrcon sqlrconref,
				int (*printfunction)(const char *,...)) {
	sqlrconref->debugPrintFunction(printfunction);
}


sqlrcur sqlrcur_alloc(sqlrcon sqlrconref) {
	sqlrcur	sqlrcurref=new sqlrcursor(sqlrconref);
	return sqlrcurref;
}

void sqlrcur_free(sqlrcur sqlrcurref) {
	delete (sqlrcur )sqlrcurref;
}

void sqlrcur_setResultSetBufferSize(sqlrcur sqlrcurref, uint64_t rows) {
	sqlrcurref->setResultSetBufferSize(rows);
}

uint64_t sqlrcur_getResultSetBufferSize(sqlrcur sqlrcurref) {
	return sqlrcurref->getResultSetBufferSize();
}

void sqlrcur_dontGetColumnInfo(sqlrcur sqlrcurref) {
	return sqlrcurref->dontGetColumnInfo();
}

void sqlrcur_getColumnInfo(sqlrcur sqlrcurref) {
	return sqlrcurref->getColumnInfo();
}

void sqlrcur_mixedCaseColumnNames(sqlrcur sqlrcurref) {
	return sqlrcurref->mixedCaseColumnNames();
}

void sqlrcur_upperCaseColumnNames(sqlrcur sqlrcurref) {
	return sqlrcurref->upperCaseColumnNames();
}

void sqlrcur_lowerCaseColumnNames(sqlrcur sqlrcurref) {
	return sqlrcurref->lowerCaseColumnNames();
}

void sqlrcur_cacheToFile(sqlrcur sqlrcurref, const char *filename) {
	sqlrcurref->cacheToFile(filename);
}

void sqlrcur_setCacheTtl(sqlrcur sqlrcurref, uint32_t ttl) {
	sqlrcurref->setCacheTtl(ttl);
}

const char *sqlrcur_getCacheFileName(sqlrcur sqlrcurref) {
	return sqlrcurref->getCacheFileName();
}

void sqlrcur_cacheOff(sqlrcur sqlrcurref) {
	sqlrcurref->cacheOff();
}

int sqlrcur_sendQuery(sqlrcur sqlrcurref, const char *query) {
	return sqlrcurref->sendQuery(query);
}

int sqlrcur_sendQueryWithLength(sqlrcur sqlrcurref, const char *query,
							uint32_t length) {
	return sqlrcurref->sendQuery(query,length);
}

int sqlrcur_sendFileQuery(sqlrcur sqlrcurref, const char *path,
							const char *filename) {
	return sqlrcurref->sendFileQuery(path,filename);
}

void sqlrcur_prepareQuery(sqlrcur sqlrcurref, const char *query) {
	sqlrcurref->prepareQuery(query);
}

void sqlrcur_prepareQueryWithLength(sqlrcur sqlrcurref, const char *query,
							uint32_t length) {
	sqlrcurref->prepareQuery(query,length);
}

void sqlrcur_prepareFileQuery(sqlrcur sqlrcurref, const char *path,
							const char *filename) {
	sqlrcurref->prepareFileQuery(path,filename);
}

void sqlrcur_subString(sqlrcur sqlrcurref, const char *variable,
							const char *value) {
	sqlrcurref->substitution(variable,value);
}

void sqlrcur_subLong(sqlrcur sqlrcurref, const char *variable, int64_t value) {
	sqlrcurref->substitution(variable,value);
}

void sqlrcur_subDouble(sqlrcur sqlrcurref, const char *variable,
			double value, uint32_t precision, uint32_t scale) {
	sqlrcurref->substitution(variable,value,precision,scale);
}

void sqlrcur_clearBinds(sqlrcur sqlrcurref) {
	sqlrcurref->clearBinds();
}

uint16_t sqlrcur_countBindVariables(sqlrcur sqlrcurref) {
	return sqlrcurref->countBindVariables();
}

void sqlrcur_inputBindString(sqlrcur sqlrcurref, const char *variable,
							const char *value) {
	sqlrcurref->inputBind(variable,value);
}

void sqlrcur_inputBindLong(sqlrcur sqlrcurref, const char *variable, 
							int64_t value) {
	sqlrcurref->inputBind(variable,value);
}

void sqlrcur_inputBindDouble(sqlrcur sqlrcurref, const char *variable, 
					double value,
					uint32_t precision, 
					uint32_t scale) {
	sqlrcurref->inputBind(variable,value,precision,scale);
}

void sqlrcur_inputBindBlob(sqlrcur sqlrcurref, const char *variable,
					const char *value, uint32_t size) {
	sqlrcurref->inputBindBlob(variable,value,size);
}

void sqlrcur_inputBindClob(sqlrcur sqlrcurref, const char *variable,
					const char *value, uint32_t size) {
	sqlrcurref->inputBindClob(variable,value,size);
}

void sqlrcur_subStrings(sqlrcur sqlrcurref,
				const char **variables, const char **values) {
	sqlrcurref->substitutions(variables,values);
}

void sqlrcur_subLongs(sqlrcur sqlrcurref, const char **variables,
						const int64_t *values) {
	sqlrcurref->substitutions(variables,values);
}

void sqlrcur_subDoubles(sqlrcur sqlrcurref,
				const char **variables,
				const double *values,
				const uint32_t *precisions,
				const uint32_t *scales) {
	sqlrcurref->substitutions(variables,values,precisions,scales);
}

void sqlrcur_inputBindStrings(sqlrcur sqlrcurref, const char **variables, 
							const char **values) {
	sqlrcurref->inputBinds(variables,values);
}

void sqlrcur_inputBindLongs(sqlrcur sqlrcurref, const char **variables, 
						const int64_t *values) {
	sqlrcurref->inputBinds(variables,values);
}

void sqlrcur_inputBindDoubles(sqlrcur sqlrcurref, 
					const char **variables,
					const double *values,
					const uint32_t *precisions, 
					const uint32_t *scales) {
	sqlrcurref->inputBinds(variables,values,precisions,scales);
}

void sqlrcur_validateBinds(sqlrcur sqlrcurref) {
	sqlrcurref->validateBinds();
}

bool sqlrcur_validBind(sqlrcur sqlrcurref, const char *variable) {
	return sqlrcurref->validBind(variable);
}

int sqlrcur_executeQuery(sqlrcur sqlrcurref) {
	return sqlrcurref->executeQuery();
}

int sqlrcur_fetchFromBindCursor(sqlrcur sqlrcurref) {
	return sqlrcurref->fetchFromBindCursor();
}

void sqlrcur_defineOutputBindString(sqlrcur sqlrcurref,
					const char *variable, uint32_t length) {
	sqlrcurref->defineOutputBindString(variable,length);
}

void sqlrcur_defineOutputBindInteger(sqlrcur sqlrcurref, const char *variable) {
	sqlrcurref->defineOutputBindInteger(variable);
}

void sqlrcur_defineOutputBindDouble(sqlrcur sqlrcurref, const char *variable) {
	sqlrcurref->defineOutputBindDouble(variable);
}

void sqlrcur_defineOutputBindBlob(sqlrcur sqlrcurref,
						const char *variable) {
	sqlrcurref->defineOutputBindBlob(variable);
}

void sqlrcur_defineOutputBindClob(sqlrcur sqlrcurref,
						const char *variable) {
	sqlrcurref->defineOutputBindClob(variable);
}

void sqlrcur_defineOutputBindCursor(sqlrcur sqlrcurref,
						const char *variable) {
	sqlrcurref->defineOutputBindCursor(variable);
}

const char *sqlrcur_getOutputBindString(sqlrcur sqlrcurref,
					const char *variable) {
	return sqlrcurref->getOutputBindString(variable);
}

const char *sqlrcur_getOutputBindBlob(sqlrcur sqlrcurref,
					const char *variable) {
	return sqlrcurref->getOutputBindBlob(variable);
}

const char *sqlrcur_getOutputBindClob(sqlrcur sqlrcurref,
					const char *variable) {
	return sqlrcurref->getOutputBindClob(variable);
}

int64_t sqlrcur_getOutputBindInteger(sqlrcur sqlrcurref,
						const char *variable) {
	return sqlrcurref->getOutputBindInteger(variable);
}

double sqlrcur_getOutputBindDouble(sqlrcur sqlrcurref,
						const char *variable) {
	return sqlrcurref->getOutputBindDouble(variable);
}

uint32_t sqlrcur_getOutputBindLength(sqlrcur sqlrcurref, const char *variable) {
	return sqlrcurref->getOutputBindLength(variable);
}

sqlrcur sqlrcur_getOutputBindCursor(sqlrcur sqlrcurref, const char *variable) {
	return sqlrcurref->getOutputBindCursor(variable);
}

int sqlrcur_openCachedResultSet(sqlrcur sqlrcurref, const char *filename) {
	return sqlrcurref->openCachedResultSet(filename);
}

uint64_t sqlrcur_rowCount(sqlrcur sqlrcurref) {
	return sqlrcurref->rowCount();
}

uint32_t sqlrcur_colCount(sqlrcur sqlrcurref) {
	return sqlrcurref->colCount();
}

uint64_t sqlrcur_totalRows(sqlrcur sqlrcurref) {
	return sqlrcurref->totalRows();
}

uint64_t sqlrcur_affectedRows(sqlrcur sqlrcurref) {
	return sqlrcurref->affectedRows();
}

uint64_t sqlrcur_firstRowIndex(sqlrcur sqlrcurref) {
	return sqlrcurref->firstRowIndex();
}

int sqlrcur_endOfResultSet(sqlrcur sqlrcurref) {
	return sqlrcurref->endOfResultSet();
}

const char *sqlrcur_errorMessage(sqlrcur sqlrcurref) {
	return sqlrcurref->errorMessage();
}

void sqlrcur_getNullsAsEmptyStrings(sqlrcur sqlrcurref) {
	sqlrcurref->getNullsAsEmptyStrings();
}

void sqlrcur_getNullsAsNulls(sqlrcur sqlrcurref) {
	sqlrcurref->getNullsAsNulls();
}

const char *sqlrcur_getFieldByIndex(sqlrcur sqlrcurref,
					uint64_t row, uint32_t col) {
	return sqlrcurref->getField(row,col);
}

const char *sqlrcur_getFieldByName(sqlrcur sqlrcurref, uint64_t row,
							const char *col) {
	return sqlrcurref->getField(row,col);
}

int64_t sqlrcur_getFieldAsIntegerByIndex(sqlrcur sqlrcurref,
					uint64_t row, uint32_t col) {
	return sqlrcurref->getFieldAsInteger(row,col);
}

int64_t sqlrcur_getFieldAsIntegerByName(sqlrcur sqlrcurref, uint64_t row,
							const char *col) {
	return sqlrcurref->getFieldAsInteger(row,col);
}

double sqlrcur_getFieldAsDoubleByIndex(sqlrcur sqlrcurref, uint64_t row,
								uint32_t col) {
	return sqlrcurref->getFieldAsDouble(row,col);
}

double sqlrcur_getFieldAsDoubleByName(sqlrcur sqlrcurref, uint64_t row,
							const char *col) {
	return sqlrcurref->getFieldAsDouble(row,col);
}

uint32_t sqlrcur_getFieldLengthByIndex(sqlrcur sqlrcurref,
					uint64_t row, uint32_t col) {
	return sqlrcurref->getFieldLength(row,col);
}

uint32_t sqlrcur_getFieldLengthByName(sqlrcur sqlrcurref,
						uint64_t row, const char *col) {
	return sqlrcurref->getFieldLength(row,col);
}

const char * const *sqlrcur_getRow(sqlrcur sqlrcurref, uint64_t row) {
	return sqlrcurref->getRow(row);
}

uint32_t *sqlrcur_getRowLengths(sqlrcur sqlrcurref, uint64_t row) {
	return sqlrcurref->getRowLengths(row);
}

const char * const *sqlrcur_getColumnNames(sqlrcur sqlrcurref) {
	return sqlrcurref->getColumnNames();
}

const char *sqlrcur_getColumnName(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnName(col);
}

const char *sqlrcur_getColumnTypeByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnType(col);
}

uint32_t sqlrcur_getColumnLengthByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnLength(col);
}

const char *sqlrcur_getColumnTypeByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnType(col);
}

uint32_t sqlrcur_getColumnLengthByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnLength(col);
}

uint32_t sqlrcur_getColumnPrecisionByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnPrecision(col);
}

uint32_t sqlrcur_getColumnPrecisionByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnPrecision(col);
}

uint32_t sqlrcur_getColumnScaleByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnScale(col);
}

uint32_t sqlrcur_getColumnScaleByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnScale(col);
}

int sqlrcur_getColumnIsNullableByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnIsNullable(col);
}

int sqlrcur_getColumnIsNullableByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnIsNullable(col);
}

int sqlrcur_getColumnIsPrimaryKeyByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnIsPrimaryKey(col);
}

int sqlrcur_getColumnIsPrimaryKeyByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnIsPrimaryKey(col);
}

int sqlrcur_getColumnIsUniqueByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnIsUnique(col);
}

int sqlrcur_getColumnIsUniqueByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnIsUnique(col);
}

int sqlrcur_getColumnIsPartOfKeyByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnIsPartOfKey(col);
}

int sqlrcur_getColumnIsPartOfKeyByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnIsPartOfKey(col);
}

int sqlrcur_getColumnIsUnsignedByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnIsUnsigned(col);
}

int sqlrcur_getColumnIsUnsignedByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnIsUnsigned(col);
}

int sqlrcur_getColumnIsZeroFilledByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnIsZeroFilled(col);
}

int sqlrcur_getColumnIsZeroFilledByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnIsZeroFilled(col);
}

int sqlrcur_getColumnIsBinaryByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnIsBinary(col);
}

int sqlrcur_getColumnIsBinaryByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnIsBinary(col);
}

int sqlrcur_getColumnIsAutoIncrementByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getColumnIsAutoIncrement(col);
}

int sqlrcur_getColumnIsAutoIncrementByName(sqlrcur sqlrcurref,
							const char *col) {
	return sqlrcurref->getColumnIsAutoIncrement(col);
}

uint32_t sqlrcur_getLongestByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getLongest(col);
}

uint32_t sqlrcur_getLongestByIndex(sqlrcur sqlrcurref, uint32_t col) {
	return sqlrcurref->getLongest(col);
}

uint16_t sqlrcur_getResultSetId(sqlrcur sqlrcurref) {
	return sqlrcurref->getResultSetId();
}

void sqlrcur_suspendResultSet(sqlrcur sqlrcurref) {
	sqlrcurref->suspendResultSet();
}

int sqlrcur_resumeResultSet(sqlrcur sqlrcurref, uint16_t id) {
	return sqlrcurref->resumeResultSet(id);
}

int sqlrcur_resumeCachedResultSet(sqlrcur sqlrcurref,
					uint16_t id, const char *filename) {
	return sqlrcurref->resumeCachedResultSet(id,filename);
}

}
