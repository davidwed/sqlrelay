// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef SQLRCURSOR_H
#define SQLRCURSOR_H

#include <defines.h>

#include <sqlrelay/sqlrclient.h>

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
			uint16_t	cursorid;
		} value;
		uint32_t	valuesize;
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
		virtual	bool	inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						int16_t *isnull);
		virtual	bool	inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		virtual	bool	inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
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
		virtual void	checkForTempTable(const char *query,
							uint32_t length);
		virtual	bool	executeQuery(const char *query,
							uint32_t length,
							bool execute)=0;
		virtual	bool		queryIsNotSelect();
		virtual	bool		queryIsCommitOrRollback();
		virtual	const char	*errorMessage(bool *liveconnection)=0;
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
		virtual	void		returnRow()=0;
		virtual	void		cleanUpData(bool freeresult,
							bool freebinds);


		// SID virtual methods

		/* method performs SQL Injection Detection */
		virtual bool	sql_injection_detection_ingress(
							const char *query);
		virtual bool	sql_injection_detection_egress();

		/* method ends sid SID database session */
		virtual void	sql_injection_detection_database_close();

		/* method maintains log for SQL Injection Detection */
		virtual void 	sql_injection_detection_log(const char *query,
							char *parsed_sql,
							char *log_buffer);

		/* method gets parameters for SQL Injection Detection */
		virtual void 	sql_injection_detection_parameters();

		/* method determines if SQL in black list */
		virtual bool	sql_injection_detection_ingress_bl(
							const char *query);
		virtual bool	sql_injection_detection_egress_bl();

		/* method determines if SQL in white list */
		virtual bool	sql_injection_detection_ingress_wl(
							const char *query);
		virtual bool	sql_injection_detection_egress_wl();

		/* method determines if SQL in learned database */
		virtual bool	sql_injection_detection_ingress_ldb();
		virtual bool	sql_injection_detection_egress_ldb();

		/* method parses the sql query */
		virtual void 	sql_injection_detection_parse_sql(
							const char *query);
		virtual void 	sql_injection_detection_parse_results(
					int32_t num_fields,
					const char * const *field_names);

		/* method to check for a row in a sid db */
		virtual bool	sql_injection_detection_check_db(
							const char *sid_db);
	

	protected:
		regularexpression	createtemp;

		// methods/variables used by derived classes
		stringbuffer	*fakeInputBinds(const char *query);

		bool	skipComment(char **ptr, const char *endptr);
		bool	skipWhitespace(char **ptr, const char *endptr);
		bool	advance(char **ptr, const char *endptr,
						uint16_t steps);

		sqlrconnection_svr	*conn;

		// variables for SID
		int32_t	sql_inject_load_params;
		int32_t	ingress_mode;
		int32_t	egress_mode;
		int32_t	listen_mode;
		int32_t	verification_mode;
		int32_t	prevention_mode;	

		char	sid_parsed_sql[BUFSIZ];
		char	sid_parsed_results[BUFSIZ];
		char	sid_query[BUFSIZ];

		char	sid_log_message[BUFSIZ];

		sqlrcursor	*sid_sqlrcur;

		bool	sql_injection_detection;

		bool	sid_egress_success;

		uint16_t	inbindcount;
		bindvar_svr	inbindvars[MAXVAR];
		uint16_t	outbindcount;
		bindvar_svr	outbindvars[MAXVAR];

	private:
		// methods used internally
		bool	handleBinds();
		void	performSubstitution(stringbuffer *buffer,
							int16_t index);
		void	abort();
		char	*skipWhitespaceAndComments(const char *querybuffer);

		char		*querybuffer;
		uint32_t	querylength;

		bool		suspendresultset;
		bool		busy;
		uint16_t	id;
};

#endif
