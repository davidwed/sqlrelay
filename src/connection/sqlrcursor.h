// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef SQLRCURSOR_H
#define SQLRCURSOR_H

#include <defines.h>

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
	friend class sqlrconnection_svr;
	public:
			sqlrcursor_svr(sqlrconnection_svr *conn);
		virtual	~sqlrcursor_svr();

		// interface definition
		virtual	bool	openCursor(uint16_t id);
		virtual	bool	closeCursor();

		virtual	bool	prepareQuery(const char *query,
						uint32_t length);
		virtual	bool	supportsNativeBinds();
		virtual	bool	inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		virtual	bool	inputBindDouble(const char *variable, 
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
						const char *tz);
		virtual bool	inputBindDate(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
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
		virtual	bool	outputBindString(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint16_t valuesize,
						int16_t *isnull);
		virtual	bool	outputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		virtual	bool	outputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		virtual bool	outputBindDate(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
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
		virtual	void	returnOutputBindBlob(uint16_t index);
		virtual	void	returnOutputBindClob(uint16_t index);
		virtual	void	returnOutputBindCursor(uint16_t index);
		virtual void	sendLobOutputBind(uint16_t index);
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
							uint32_t length,
							bool execute)=0;
		virtual	bool		queryIsNotSelect();
		virtual	bool		queryIsCommitOrRollback();
		virtual	void		errorMessage(const char **errorstring,
							int64_t *errorcode,
							bool *liveconnection)=0;
		virtual bool		knowsRowCount()=0;
		virtual uint64_t	rowCount()=0;
		virtual bool		knowsAffectedRows()=0;
		virtual uint64_t	affectedRows()=0;
		virtual	uint32_t	colCount()=0;
		virtual const char * const * columnNames()=0;
		virtual uint16_t	columnTypeFormat()=0;
		virtual	void		returnColumnInfo()=0;
		virtual	bool		noRowsToReturn()=0;
		virtual	bool		skipRow()=0;
		virtual	bool		fetchRow()=0;
		virtual	void		returnRow();
		virtual	void		nextRow();
		virtual void		getField(uint32_t col,
							const char **field,
							uint64_t *fieldlength,
							bool *blob,
							bool *null);
		virtual void		sendLobField(uint32_t col);
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

		virtual bool		translateQuery();

		virtual bool		getColumnNameList(stringbuffer *output);


		// SID virtual methods

		// method performs SQL Injection Detection
		virtual bool	sql_injection_detection_ingress(
							const char *query);
		virtual bool	sql_injection_detection_egress();

		// method maintains log for SQL Injection Detection
		virtual void 	sql_injection_detection_log(const char *query,
							const char *parsed_sql,
							const char *log_buffer);

		// method gets parameters for SQL Injection Detection
		virtual void 	sql_injection_detection_parameters();

		// method determines if SQL in black list
		virtual bool	sql_injection_detection_ingress_bl(
							const char *query);
		virtual bool	sql_injection_detection_egress_bl();

		// method determines if SQL in white list
		virtual bool	sql_injection_detection_ingress_wl(
							const char *query);
		virtual bool	sql_injection_detection_egress_wl();

		// method determines if SQL in learned database
		virtual bool	sql_injection_detection_ingress_ldb();
		virtual bool	sql_injection_detection_egress_ldb();

		// method parses the sql query
		virtual void 	sql_injection_detection_parse_sql(
							const char *query);
		virtual void 	sql_injection_detection_parse_results(
					int32_t num_fields,
					const char * const *field_names);

		// method to check for a row in a sid db
		virtual bool	sql_injection_detection_check_db(
							const char *sid_db);

		void		setFakeInputBindsForThisQuery(bool fake);

		void	printQueryTree(xmldom *tree);
	
	protected:
		// methods/variables used by derived classes
		bool	skipComment(char **ptr, const char *endptr);
		bool	skipWhitespace(char **ptr, const char *endptr);
		char	*skipWhitespaceAndComments(const char *querybuffer);
		stringbuffer	*fakeInputBinds(const char *query);

		bool	advance(char **ptr, const char *endptr,
						uint16_t steps);

		sqlrconnection_svr	*conn;
		regularexpression	createtemp;

		// variables for SID
		bool	ingress_mode;
		bool	egress_mode;
		bool	listen_mode;
		bool	verification_mode;
		bool	prevention_mode;	

		char	sid_parsed_sql[BUFSIZ];
		char	sid_parsed_results[BUFSIZ];
		char	sid_query[BUFSIZ];

		sqlrcursor	*sid_sqlrcur;

		bool	sql_injection_detection;

		bool	sid_egress;

		char		*querybuffer;
		uint32_t	querylength;
		xmldom		*querytree;

		bool	fakeinputbindsforthisquery;

	// ideally these would be protected but the
	// translators and triggers need to access them (for now)
	public:
		uint16_t	inbindcount;
		bindvar_svr	inbindvars[MAXVAR];
		uint16_t	outbindcount;
		bindvar_svr	outbindvars[MAXVAR];

		// this one too...
		bool	openCursorInternal(uint16_t id);

	private:
		// methods used internally
		bool	handleBinds();
		void	performSubstitution(stringbuffer *buffer,
							int16_t index);
		void	abort();

		uint64_t	querysec;
		uint64_t	queryusec;

		bool		suspendresultset;
		bool		busy;
		uint16_t	id;

		char		lobbuffer[32768];
};

#endif
