// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ROUTERCONNECTION_H
#define ROUTERCONNECTION_H

#define FETCH_AT_ONCE	10

#include <sqlrconnection.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/regularexpression.h>

struct outputbindvar {
	const char	*variable;
	union {
		char		*stringvalue;
		int64_t		*intvalue;
		double		*doublevalue;
		struct {
			int16_t		*year;
			int16_t		*month;
			int16_t		*day;
			int16_t		*hour;
			int16_t		*minute;
			int16_t		*second;
			int32_t		*microsecond;
			const char	**tz;
		} datevalue;
	} value;
	uint16_t	valuesize;
	bindtype	type;
	int16_t		*isnull;
};

struct cursorbindvar {
	const char	*variable;
	sqlrcursor_svr	*cursor;
};

class routerconnection;

class routercursor : public sqlrcursor_svr {
	friend class routerconnection;
	private:
				routercursor(sqlrconnection_svr *conn);
				~routercursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		supportsNativeBinds();
		bool		begin(const char *query, uint32_t length);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		bool		inputBind(const char *variable,
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
		bool		inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint16_t valuesize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBind(const char *variable,
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
		bool		outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrcursor_svr *cursor);
		bool		getLobOutputBindLength(uint16_t index,
						uint64_t *length);
		bool		getLobOutputBindSegment(uint16_t index,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);
		bool		executeQuery(const char *query,
						uint32_t length);
		void		checkForTempTable(const char *query,
							uint32_t length);
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char * const * columnNames();
		uint16_t	columnTypeFormat();
		const char	*getColumnName(uint32_t col);
		const char	*getColumnTypeName(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		uint16_t	getColumnIsNullable(uint32_t col);
		uint16_t	getColumnIsPrimaryKey(uint32_t col);
		uint16_t	getColumnIsUnique(uint32_t col);
		uint16_t	getColumnIsPartOfKey(uint32_t col);
		uint16_t	getColumnIsUnsigned(uint32_t col);
		uint16_t	getColumnIsZeroFilled(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		uint16_t	getColumnIsAutoIncrement(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		cleanUpData(bool freeresult, bool freebinds);

		routerconnection	*routerconn;

		sqlrconnection	*con;
		sqlrcursor	*cur;
		bool		isbindcur;
		uint16_t	curindex;
		sqlrcursor	**curs;

		uint64_t	nextrow;

		bool		beginquery;

		outputbindvar	*obv;
		uint16_t	obcount;

		cursorbindvar	*cbv;
		uint16_t	cbcount;

		regularexpression	createoratemp;
		regularexpression	preserverows;
};

class routerconnection : public sqlrconnection_svr {
	friend class routercursor;
	public:
			routerconnection();
			~routerconnection();
	private:
		bool		supportsAuthOnDatabase();
		void		handleConnectString();
		bool		logIn(bool printerrors);
		sqlrcursor_svr	*initCursor();
		void		deleteCursor(sqlrcursor_svr *curs);
		void		logOut();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		const char	*identify();
		const char	*dbVersion();
		bool		ping();
		bool		getLastInsertId(uint64_t *id);
		void		endSession();

		void	autoCommitOnFailed(uint16_t index);
		void	autoCommitOffFailed(uint16_t index);
		void	commitFailed(uint16_t index);
		void	rollbackFailed(uint16_t index);
		void	beginQueryFailed(uint16_t index);

		sqlrconnection	**cons;
		sqlrconnection	*cur;
		const char	**beginquery;
		bool		anymustbegin;
		uint16_t	concount;

		sqlrconfigfile	*cfgfile;

		bool		justloggedin;

		int16_t		nullbindvalue;
		int16_t		nonnullbindvalue;

		regularexpression	beginregex;
		regularexpression	beginendregex;

		const char	*error;
};

#endif
