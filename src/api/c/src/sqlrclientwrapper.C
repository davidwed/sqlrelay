/* Copyright (c) 2000-2001  David Muse
   See the file COPYING for more information */

#include <sqlrelay/sqlrclient.h>

extern "C" {
	#undef __cplusplus
	#include <string.h>
	#include <stdarg.h>
	#ifdef HAVE_STRINGS_H
		#include <strings.h>
	#endif
	#define __cplusplus

#include <sqlrelay/sqlrclientwrapper.h>

sqlrcon	sqlrcon_alloc(const char *server, int port, const char *socket,
				const char *user, const char *password, 
				int retrytime, int tries) {
	sqlrcon	sqlrconref=new sqlrconnection(server,port,socket,
					user,password,retrytime,tries);
	return sqlrconref;
}

void	sqlrcon_free(sqlrcon sqlrconref) {
	delete (sqlrconnection *)sqlrconref;
}

int	sqlrcon_endSession(sqlrcon sqlrconref) {
	return sqlrconref->endSession();
}

int	sqlrcon_suspendSession(sqlrcon sqlrconref) {
	return sqlrconref->suspendSession();
}

int	sqlrcon_getConnectionPort(sqlrcon sqlrconref) {
	return sqlrconref->getConnectionPort();
}

char	*sqlrcon_getConnectionSocket(sqlrcon sqlrconref) {
	return sqlrconref->getConnectionSocket();
}

int	sqlrcon_resumeSession(sqlrcon sqlrconref, int port,
					const char *socket) {
	return sqlrconref->resumeSession(port,socket);
}

int	sqlrcon_ping(sqlrcon sqlrconref) {
	return sqlrconref->ping();
}

char	*sqlrcon_identify(sqlrcon sqlrconref) {
	return sqlrconref->identify();
}

int	sqlrcon_autoCommitOn(sqlrcon sqlrconref) {
	return sqlrconref->autoCommitOn();
}

int	sqlrcon_autoCommitOff(sqlrcon sqlrconref) {
	return sqlrconref->autoCommitOff();
}

int	sqlrcon_commit(sqlrcon sqlrconref) {
	return sqlrconref->commit();
}

int	sqlrcon_rollback(sqlrcon sqlrconref) {
	return sqlrconref->rollback();
}

void	sqlrcon_debugOn(sqlrcon sqlrconref) {
	sqlrconref->debugOn();
}

void	sqlrcon_debugOff(sqlrcon sqlrconref) {
	sqlrconref->debugOff();
}

int	sqlrcon_getDebug(sqlrcon sqlrconref) {
	return sqlrconref->getDebug();
}

void	sqlrcon_debugPrintFunction(sqlrcon sqlrconref,
				int (*printfunction)(const char *,...)) {
	sqlrconref->debugPrintFunction(printfunction);
}


sqlrcur	sqlrcur_alloc(sqlrcon sqlrconref) {
	sqlrcur	sqlrcurref=new sqlrcursor(sqlrconref);
	return sqlrcurref;
}

void	sqlrcur_free(sqlrcur sqlrcurref) {
	delete (sqlrcur )sqlrcurref;
}

void	sqlrcur_setResultSetBufferSize(sqlrcur sqlrcurref, int rows) {
	sqlrcurref->setResultSetBufferSize(rows);
}

int	sqlrcur_getResultSetBufferSize(sqlrcur sqlrcurref) {
	return sqlrcurref->getResultSetBufferSize();
}

void	sqlrcur_dontGetColumnInfo(sqlrcur sqlrcurref) {
	return sqlrcurref->dontGetColumnInfo();
}

void	sqlrcur_getColumnInfo(sqlrcur sqlrcurref) {
	return sqlrcurref->getColumnInfo();
}

void	sqlrcur_mixedCaseColumnNames(sqlrcur sqlrcurref) {
	return sqlrcurref->mixedCaseColumnNames();
}

void	sqlrcur_upperCaseColumnNames(sqlrcur sqlrcurref) {
	return sqlrcurref->upperCaseColumnNames();
}

void	sqlrcur_lowerCaseColumnNames(sqlrcur sqlrcurref) {
	return sqlrcurref->lowerCaseColumnNames();
}

void	sqlrcur_cacheToFile(sqlrcur sqlrcurref, const char *filename) {
	sqlrcurref->cacheToFile(filename);
}

void	sqlrcur_setCacheTtl(sqlrcur sqlrcurref, int ttl) {
	sqlrcurref->setCacheTtl(ttl);
}

char	*sqlrcur_getCacheFileName(sqlrcur sqlrcurref) {
	return sqlrcurref->getCacheFileName();
}

void	sqlrcur_cacheOff(sqlrcur sqlrcurref) {
	sqlrcurref->cacheOff();
}

int	sqlrcur_sendQuery(sqlrcur sqlrcurref, const char *query) {
	return sqlrcurref->sendQuery(query);
}

int	sqlrcur_sendQueryWithLength(sqlrcur sqlrcurref, const char *query,
								int length) {
	return sqlrcurref->sendQuery(query,length);
}

int	sqlrcur_sendFileQuery(sqlrcur sqlrcurref, const char *path,
							const char *filename) {
	return sqlrcurref->sendFileQuery(path,filename);
}

void	sqlrcur_prepareQuery(sqlrcur sqlrcurref, const char *query) {
	sqlrcurref->prepareQuery(query);
}

void	sqlrcur_prepareQueryWithLength(sqlrcur sqlrcurref, const char *query,
								int length) {
	sqlrcurref->prepareQuery(query,length);
}

void	sqlrcur_prepareFileQuery(sqlrcur sqlrcurref, const char *path,
							const char *filename) {
	sqlrcurref->prepareFileQuery(path,filename);
}

void	sqlrcur_subString(sqlrcur sqlrcurref, const char *variable,
							const char *value) {
	sqlrcurref->substitution(variable,value);
}

void	sqlrcur_subLong(sqlrcur sqlrcurref, const char *variable, long value) {
	sqlrcurref->substitution(variable,value);
}

void	sqlrcur_subDouble(sqlrcur sqlrcurref, const char *variable,
			double value, 
			unsigned short precision, unsigned short scale) {
	sqlrcurref->substitution(variable,value,precision,scale);
}

void	sqlrcur_clearBinds(sqlrcur sqlrcurref) {
	sqlrcurref->clearBinds();
}

void	sqlrcur_inputBindString(sqlrcur sqlrcurref, const char *variable,
							const char *value) {
	sqlrcurref->inputBind(variable,value);
}

void	sqlrcur_inputBindLong(sqlrcur sqlrcurref, const char *variable, 
						unsigned long value) {
	sqlrcurref->inputBind(variable,value);
}

void	sqlrcur_inputBindDouble(sqlrcur sqlrcurref, const char *variable, 
					double value,
					unsigned short precision, 
					unsigned short scale) {
	sqlrcurref->inputBind(variable,value,precision,scale);
}

void	sqlrcur_inputBindBlob(sqlrcur sqlrcurref, const char *variable,
					const char *value, unsigned long size) {
	sqlrcurref->inputBindBlob(variable,value,size);
}

void	sqlrcur_inputBindClob(sqlrcur sqlrcurref, const char *variable,
					const char *value, unsigned long size) {
	sqlrcurref->inputBindClob(variable,value,size);
}

void	sqlrcur_subStrings(sqlrcur sqlrcurref,
				const char **variables, const char **values) {
	sqlrcurref->substitutions(variables,values);
}

void	sqlrcur_subLongs(sqlrcur sqlrcurref, const char **variables,
						const long *values) {
	sqlrcurref->substitutions(variables,values);
}

void	sqlrcur_subDoubles(sqlrcur sqlrcurref,
				const char **variables,
				const double *values,
				const unsigned short *precisions,
				const unsigned short *scales) {
	sqlrcurref->substitutions(variables,values,precisions,scales);
}

void	sqlrcur_inputBindStrings(sqlrcur sqlrcurref, const char **variables, 
							const char **values) {
	sqlrcurref->inputBinds(variables,values);
}

void	sqlrcur_inputBindLongs(sqlrcur sqlrcurref, const char **variables, 
						const unsigned long *values) {
	sqlrcurref->inputBinds(variables,values);
}

void	sqlrcur_inputBindDoubles(sqlrcur sqlrcurref, 
					const char **variables,
					const double *values,
					const unsigned short *precisions, 
					const unsigned short *scales) {
	sqlrcurref->inputBinds(variables,values,precisions,scales);
}

void	sqlrcur_validateBinds(sqlrcur sqlrcurref) {
	sqlrcurref->validateBinds();
}

int	sqlrcur_executeQuery(sqlrcur sqlrcurref) {
	return sqlrcurref->executeQuery();
}

int	sqlrcur_fetchFromBindCursor(sqlrcur sqlrcurref) {
	return sqlrcurref->fetchFromBindCursor();
}

void	sqlrcur_defineOutputBind(sqlrcur sqlrcurref, const char *variable,
							unsigned long length) {
	sqlrcurref->defineOutputBind(variable,length);
}

void	sqlrcur_defineOutputBindBlob(sqlrcur sqlrcurref,
						const char *variable) {
	sqlrcurref->defineOutputBindBlob(variable);
}

void	sqlrcur_defineOutputBindClob(sqlrcur sqlrcurref,
						const char *variable) {
	sqlrcurref->defineOutputBindClob(variable);
}

void	sqlrcur_defineOutputBindCursor(sqlrcur sqlrcurref,
						const char *variable) {
	sqlrcurref->defineOutputBindCursor(variable);
}

char	*sqlrcur_getOutputBind(sqlrcur sqlrcurref, const char *variable) {
	return sqlrcurref->getOutputBind(variable);
}

long	sqlrcur_getOutputBindLength(sqlrcur sqlrcurref, const char *variable) {
	return sqlrcurref->getOutputBindLength(variable);
}

sqlrcur	sqlrcur_getOutputBindCursor(sqlrcur sqlrcurref, const char *variable) {
	return sqlrcurref->getOutputBindCursor(variable);
}

int	sqlrcur_openCachedResultSet(sqlrcur sqlrcurref, const char *filename) {
	return sqlrcurref->openCachedResultSet(filename);
}

int	sqlrcur_rowCount(sqlrcur sqlrcurref) {
	return sqlrcurref->rowCount();
}

int	sqlrcur_colCount(sqlrcur sqlrcurref) {
	return sqlrcurref->colCount();
}

int	sqlrcur_totalRows(sqlrcur sqlrcurref) {
	return sqlrcurref->totalRows();
}

int	sqlrcur_affectedRows(sqlrcur sqlrcurref) {
	return sqlrcurref->affectedRows();
}

int	sqlrcur_firstRowIndex(sqlrcur sqlrcurref) {
	return sqlrcurref->firstRowIndex();
}

int	sqlrcur_endOfResultSet(sqlrcur sqlrcurref) {
	return sqlrcurref->endOfResultSet();
}

char	*sqlrcur_errorMessage(sqlrcur sqlrcurref) {
	return sqlrcurref->errorMessage();
}

void	sqlrcur_getNullsAsEmptyStrings(sqlrcur sqlrcurref) {
	sqlrcurref->getNullsAsEmptyStrings();
}

void	sqlrcur_getNullsAsNulls(sqlrcur sqlrcurref) {
	sqlrcurref->getNullsAsNulls();
}

char	*sqlrcur_getFieldByIndex(sqlrcur sqlrcurref, int row, int col) {
	return sqlrcurref->getField(row,col);
}

char	*sqlrcur_getFieldByName(sqlrcur sqlrcurref, int row, const char *col) {
	return sqlrcurref->getField(row,col);
}

long	sqlrcur_getFieldLengthByIndex(sqlrcur sqlrcurref, int row, int col) {
	return sqlrcurref->getFieldLength(row,col);
}

long	sqlrcur_getFieldLengthByName(sqlrcur sqlrcurref,
						int row, const char *col) {
	return sqlrcurref->getFieldLength(row,col);
}

char	**sqlrcur_getRow(sqlrcur sqlrcurref, int row) {
	return sqlrcurref->getRow(row);
}

long	*sqlrcur_getRowLengths(sqlrcur sqlrcurref, int row) {
	return sqlrcurref->getRowLengths(row);
}

char	**sqlrcur_getColumnNames(sqlrcur sqlrcurref) {
	return sqlrcurref->getColumnNames();
}

char	*sqlrcur_getColumnName(sqlrcur sqlrcurref, int col) {
	return sqlrcurref->getColumnName(col);
}

char	*sqlrcur_getColumnTypeByIndex(sqlrcur sqlrcurref, int col) {
	return sqlrcurref->getColumnType(col);
}

int	sqlrcur_getColumnLengthByIndex(sqlrcur sqlrcurref, int col) {
	return sqlrcurref->getColumnLength(col);
}

char	*sqlrcur_getColumnTypeByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnType(col);
}

int	sqlrcur_getColumnLengthByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getColumnLength(col);
}

int	sqlrcur_getLongestByName(sqlrcur sqlrcurref, const char *col) {
	return sqlrcurref->getLongest(col);
}

int	sqlrcur_getLongestByIndex(sqlrcur sqlrcurref, int col) {
	return sqlrcurref->getLongest(col);
}

int	sqlrcur_getResultSetId(sqlrcur sqlrcurref) {
	return sqlrcurref->getResultSetId();
}

void	sqlrcur_suspendResultSet(sqlrcur sqlrcurref) {
	sqlrcurref->suspendResultSet();
}

int	sqlrcur_resumeResultSet(sqlrcur sqlrcurref, int id) {
	return sqlrcurref->resumeResultSet(id);
}

int	sqlrcur_resumeCachedResultSet(sqlrcur sqlrcurref,
					int id, const char *filename) {
	return sqlrcurref->resumeCachedResultSet(id,filename);
}

}
