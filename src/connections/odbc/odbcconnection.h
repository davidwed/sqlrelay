// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ODBCCCONNECTION_H
#define ODBCCCONNECTION_H

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096

#define NUM_CONNECT_STRING_VARS 3

#ifdef HAVE_IODBC
	#include <iodbcinst.h>
#endif
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

// note that sqlrconnection.h must be included after sqltypes.h to
// get around a problem with CHAR/xmlChar in gnome-xml
#include <sqlrconnection.h>

#include <config.h>

struct odbccolumn {
	char		name[MAX_ITEM_BUFFER_SIZE];
	uint16_t	namelength;
	// SQLColAttribute requires that these are signed, 32 bit integers
	int32_t		type;
	int32_t		length;
	int32_t		precision;
	int32_t		scale;
	int32_t		nullable;
	uint16_t	primarykey;
	uint16_t	unique;
	uint16_t	partofkey;
	uint16_t	unsignednumber;
	uint16_t	zerofill;
	uint16_t	binary;
	uint16_t	autoincrement;
};

struct datebind {
	int16_t		*year;
	int16_t		*month;
	int16_t		*day;
	int16_t		*hour;
	int16_t		*minute;
	int16_t		*second;
	int32_t		*microsecond;
	const char	**tz;
	char		*buffer;
};

class odbcconnection;

class odbccursor : public sqlrcursor_svr {
	friend class odbcconnection;
	private:
				odbccursor(sqlrconnection_svr *conn);
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		allocateStatementHandle();
		void		initializeRowAndColumnCounts();
		bool		inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						short *isnull);
		bool		inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value, 
						uint32_t precision,
						uint32_t scale);
		bool		inputBindDate(const char *variable,
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
						int16_t *isnull);
		bool		outputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						short *isnull);
		bool		outputBindInteger(const char *variable,
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBindDouble(const char *variable,
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBindDate(const char *variable,
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
						int16_t *isnull);
		short		nonNullBindValue();
		short		nullBindValue();
		bool		bindValueIsNull(short isnull);
		bool		executeQuery(const char *query,
						uint32_t length,
						bool execute);
		bool		handleColumns();
		void		errorMessage(const char **errorstring,
						int64_t	*errorcode,
						bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		bool		knowsAffectedRows();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char * const * columnNames();
		uint16_t	columnTypeFormat();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		nextRow();
		void		cleanUpData(bool freeresult, bool freebinds);


		SQLRETURN	erg;
		SQLHSTMT	stmt;
		SQLSMALLINT	ncols;
		SQLINTEGER 	affectedrows;

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#ifdef HAVE_UNIXODBC
		char		field[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE]
						[FETCH_AT_ONCE];
#else*/
		char		field[MAX_SELECT_LIST_SIZE]
					[MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE];
//#endif
		odbccolumn 	col[MAX_SELECT_LIST_SIZE];
		char		*columnnames[MAX_SELECT_LIST_SIZE];

		datebind	*outdatebind[MAXVAR];

		uint32_t	row;
		uint32_t	maxrow;
		uint32_t	totalrows;
		uint32_t	rownumber;

		stringbuffer	errormsg;

		odbcconnection	*odbcconn;
};

class odbcconnection : public sqlrconnection_svr {
	friend class odbccursor;
	private:
		uint16_t	getNumberOfConnectStringVars();
		void		handleConnectString();
		bool		logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
#if (ODBCVER>=0x0300)
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		void		errorMessage(const char **errorstring,
						int64_t	*errorcode,
						bool *liveconnection);
#endif
		bool		ping();
		const char	*identify();
		const char	*dbVersion();
		bool		getListsByApiCalls();
		bool		getDatabaseList(sqlrcursor_svr *cursor,
						const char *wild);
		bool		getTableList(sqlrcursor_svr *cursor,
						const char *wild);
		bool		getDatabaseOrTableList(
						sqlrcursor_svr *cursor,
						const char *wild,
						bool table);
		bool		getColumnList(sqlrcursor_svr *cursor,
						const char *table,
						const char *wild);
		bool		setIsolationLevel(const char *isolevel);

		SQLRETURN	erg;
		SQLHENV		env;
		SQLHDBC		dbc;

		const char	*dsn;

		char		dbversion[512];

#if (ODBCVER>=0x0300)
		stringbuffer	errormsg;
#endif
};

#endif
