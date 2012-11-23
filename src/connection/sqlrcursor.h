// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef SQLRCURSOR_H
#define SQLRCURSOR_H

#include <defines.h>

#include <sqlrquery.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/regularexpression.h>
#include <rudiments/xmldom.h>

class bindvar_svr {
	public:
		char	*variable;
		int16_t	variablesize;
		union {
			char	*stringval;
			int64_t	integerval;
			struct	{
				double		value;
				uint32_t	precision;
				uint32_t	scale;
			} doubleval;
			struct {
				int16_t		year;
				int16_t		month;
				int16_t		day;
				int16_t		hour;
				int16_t		minute;
				int16_t		second;
				int32_t		microsecond;
				char		*tz;
				char		*buffer;
				uint16_t	buffersize;
			} dateval;
			uint16_t	cursorid;
		} value;
		uint32_t	valuesize;
		uint32_t	resultvaluesize;
		bindtype	type;
		int16_t		isnull;
};

class sqlrconnection_svr;

class sqlrcursor_svr {
	friend class sqlrcontroller_svr;
	friend class sqlrconnection_svr;
	public:
			sqlrcursor_svr(sqlrconnection_svr *conn);
		virtual	~sqlrcursor_svr();

		// interface definition
		virtual	bool	open(uint16_t id);
		virtual	bool	close();

		virtual	bool	prepareQuery(const char *query,
						uint32_t length);
		virtual	bool	supportsNativeBinds();
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		virtual	bool	inputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		virtual void	dateToString(char *buffer,
						uint16_t buffersize,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz);
		virtual bool	inputBind(const char *variable,
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
		virtual	bool	inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint16_t valuesize,
						int16_t *isnull);
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		virtual	bool	outputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		virtual bool	outputBind(const char *variable,
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
		virtual	bool	outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		virtual	bool	outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		virtual	bool	outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrcursor_svr *cursor);
		virtual bool	getLobOutputBindLength(uint16_t index,
							uint64_t *length);
		virtual bool	getLobOutputBindSegment(uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		virtual void	checkForTempTable(const char *query,
							uint32_t length);
		virtual	bool	executeQuery(const char *query,
							uint32_t length);
		virtual bool	fetchFromBindCursor();
		virtual	bool		queryIsNotSelect();
		virtual	bool		queryIsCommitOrRollback();
		virtual	void		errorMessage(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection);
		virtual bool		knowsRowCount();
		virtual uint64_t	rowCount();
		virtual bool		knowsAffectedRows();
		virtual uint64_t	affectedRows();
		virtual	uint32_t	colCount();
		virtual uint16_t	columnTypeFormat();
		virtual const char * const * columnNames();
		virtual const char	*getColumnName(uint32_t col);
		virtual uint16_t	getColumnNameLength(uint32_t col);
		virtual uint16_t	getColumnType(uint32_t col);
		virtual const char	*getColumnTypeName(uint32_t col);
		virtual uint16_t	getColumnTypeNameLength(uint32_t col);
		virtual uint32_t	getColumnLength(uint32_t col);
		virtual uint32_t	getColumnPrecision(uint32_t col);
		virtual uint32_t	getColumnScale(uint32_t col);
		virtual uint16_t	getColumnIsNullable(uint32_t col);
		virtual uint16_t	getColumnIsPrimaryKey(uint32_t col);
		virtual uint16_t	getColumnIsUnique(uint32_t col);
		virtual uint16_t	getColumnIsPartOfKey(uint32_t col);
		virtual uint16_t	getColumnIsUnsigned(uint32_t col);
		virtual uint16_t	getColumnIsZeroFilled(uint32_t col);
		virtual uint16_t	getColumnIsBinary(uint32_t col);
		virtual uint16_t	getColumnIsAutoIncrement(uint32_t col);
		virtual	bool		noRowsToReturn();
		virtual	bool		skipRow();
		virtual	bool		fetchRow();
		virtual	void		nextRow();
		virtual void		getField(uint32_t col,
							const char **field,
							uint64_t *fieldlength,
							bool *blob,
							bool *null);
		virtual bool		getLobFieldLength(uint32_t col,
							uint64_t *length);
		virtual bool		getLobFieldSegment(uint32_t col,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		virtual void		cleanUpLobField(uint32_t col);
		virtual	void		cleanUpData(bool freeresult,
							bool freebinds);
		virtual bool		getColumnNameList(stringbuffer *output);

		void	setFakeInputBindsForThisQuery(bool fake);
	
	protected:
		// methods/variables used by derived classes
		bool	skipComment(char **ptr, const char *endptr);
		bool	skipWhitespace(char **ptr, const char *endptr);
		char	*skipWhitespaceAndComments(const char *querybuffer);
		bool	fakeInputBinds(stringbuffer *outputquery);

		void	clearError();
		void	setError(const char *err, int64_t errn, bool liveconn);

		sqlrconnection_svr	*conn;
		regularexpression	createtemp;

	// ideally these would be protected but the translators,
	// triggers and loggers need to access them (for now)
	public:
		char		*querybuffer;
		uint32_t	querylength;
		xmldom		*querytree;
		bool		queryresult;

		char		*error;
		uint32_t	errorlength;
		int64_t		errnum;
		bool		liveconnection;

		int64_t		commandstartsec;
		int64_t		commandstartusec;
		int64_t		querystartsec;
		int64_t		querystartusec;
		int64_t		queryendsec;
		int64_t		queryendusec;
		int64_t		commandendsec;
		int64_t		commandendusec;

		bool	fakeinputbindsforthisquery;

		uint16_t	inbindcount;
		bindvar_svr	*inbindvars;
		uint16_t	outbindcount;
		bindvar_svr	*outbindvars;

		bool		lastrowvalid;
		uint64_t	lastrow;

		// this one too...
		bool	openInternal(uint16_t id);

	private:
		// methods used internally
		void	performSubstitution(stringbuffer *buffer,
							int16_t index);
		void	abort();

		bool		suspendresultset;
		bool		busy;
		uint16_t	id;

		char		lobbuffer[32768];
};

#endif
