// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

	private:
		void	init(sqlrconnection *sqlrc, bool copyreferences);
		void	clearConnectionPorts();
		void	clearResultSet();
		void	clearCacheDest();
		void	clearCacheSource();
		void	clearError();
		void	clearColumns();
		void	clearRows();
		void	clearVariables();
		void	clearVariables(bool clearbinds);
		void	deleteSubstitutionVariables();
		void	deleteInputBindVariables();
		void	deleteOutputBindVariables();
		void	deleteInputOutputBindVariables();
		void	deleteVariables();

		void	initQueryBuffer(uint32_t querylength);
		bool	sendQueryInternal();
		bool	getList(uint16_t command,
				sqlrclientlistformat_t listformat,
				const char *table, const char *wild,
				uint16_t objecttypes);
		void	sendCursorStatus();
		void	performSubstitutions();
		void	validateBindsInternal();
		bool	validateBind(const char *variable);
		void	sendInputBinds();
		void	sendOutputBinds();
		void	sendInputOutputBinds();
		void	sendGetColumnInfo();
		void	defineOutputBindGeneric(const char *variable,
						sqlrclientbindvartype_t type,
						uint32_t valuesize);
		void	stringVar(sqlrclientbindvar *var,
						const char *variable,
						const char *value);
		void	stringVar(sqlrclientbindvar *var,
						const char *variable,
						const char *value,
						uint32_t valuesize);
		void	integerVar(sqlrclientbindvar *var,
						const char *variable,
						int64_t value);
		void	doubleVar(sqlrclientbindvar *var,
						const char *variable, 
						double value,
						uint32_t precision,
						uint32_t scale);
		void	dateVar(sqlrclientbindvar *var,
						const char *variable,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative);
		void	lobVar(sqlrclientbindvar *var,
						const char *variable,
						const char *value,
						uint32_t size,
						sqlrclientbindvartype_t type);
		void	initVar(sqlrclientbindvar *var,
						const char *variable,
						bool preexisting);
		void	performSubstitution(stringbuffer *buffer,
							uint16_t which);
		bool	runQuery();
		bool	processInitialResultSet();

		int32_t	getString(char *string, int32_t size);
		int32_t	getBool(bool *boolean);
		int32_t	getShort(uint16_t *integer);
		int32_t	getShort(uint16_t *integer,
				int32_t timeoutsec, int32_t timeoutusec);
		int32_t	getLong(uint32_t *integer);
		int32_t	getLongLong(uint64_t *integer);
		int32_t	getDouble(double *value);

		uint16_t	getErrorStatus();

		bool	getCursorId();
		bool	getSuspended();
		bool	parseColumnInfo();
		bool	parseOutputBinds();
		bool	parseInputOutputBinds();
		bool	parseResults();
		void	setError(const char *err);
		void	getErrorFromServer();
		void	handleError();

		bool	skipAndFetch(bool initial, uint64_t rowstoskip);
		bool	skipRows(bool initial, uint64_t rowstoskip);
		void	fetchRows();

		void	startCaching();
		void	cacheError();
		void	cacheNoError();
		void	cacheColumnInfo();
		void	cacheOutputBinds(uint32_t count);
		void	cacheInputOutputBinds(uint32_t count);
		void	cacheData();
		void	finishCaching();
 
		bool	fetchRowIntoBuffer(uint64_t row,
						uint64_t *rowbufferindex);

		void	createColumnArrays();
		void	createExtraRowArray();
		void	createFields();
		void	createFieldLengths();

		char		*getFieldInternal(uint64_t row,
							uint32_t col);
		uint32_t	getFieldLengthInternal(uint64_t row,
							uint32_t col);

		char	*getRowStorage(int32_t length);
		void	createRowBuffers();
		sqlrclientcolumn	*getColumn(uint32_t index);
		sqlrclientcolumn	*getColumn(const char *name);
		sqlrclientcolumn	*getColumnInternal(uint32_t index);
		char	*getColStorage(int32_t length);
		void	createColumnBuffers();

		void	closeResultSet(bool closeremote);

		bool		endofresultset();
		void		sqlrc(sqlrconnection *sqlrc);
		sqlrcursor	*next();
		void		havecursorid(bool havecursorid);

		sqlrcursorprivate	*pvt;

	public:
		sqlrcursor(sqlrconnection *sqlrcon, bool copyreferences);
		bool		outputBindCursorIdIsValid(const char *variable);
		sqlrcursor	*getOutputBindCursor(const char *variable,
								bool copyrefs);
		uint16_t	getOutputBindCursorId(const char *variable);
		void		attachToBindCursor(uint16_t bindcursorid);
		const char	*getQueryTree();
		const char	*getTranslatedQuery();

		bool		getDatabaseList(const char *wild,
					sqlrclientlistformat_t listformat);
		bool		getSchemaList(const char *wild);
		bool		getSchemaList(const char *wild,
					sqlrclientlistformat_t listformat);
		bool		getTableList(const char *wild,
					sqlrclientlistformat_t listformat,
					uint16_t objecttypes);
		bool		getTableTypeList(const char *wild);
		bool		getTableTypeList(const char *wild,
					sqlrclientlistformat_t listformat);
		bool		getColumnList(const char *table,
					const char *wild,
					sqlrclientlistformat_t listformat);
		bool		getPrimaryKeysList(const char *table,
					const char *wild);
		bool		getPrimaryKeysList(const char *table,
					const char *wild,
					sqlrclientlistformat_t listformat);
		bool		getKeyAndIndexList(const char *table,
					const char *wild);
		bool		getKeyAndIndexList(const char *table,
					const char *wild,
					sqlrclientlistformat_t listformat);
		bool		getProcedureBindAndColumnList(
					const char *procedure,
					const char *wild);
		bool		getProcedureBindAndColumnList(
					const char *procedure,
					const char *wild,
					sqlrclientlistformat_t listformat);
		bool		getTypeInfoList(
					const char *type,
					const char *wild);
		bool		getTypeInfoList(
					const char *type,
					const char *wild,
					sqlrclientlistformat_t listformat);
		bool		getProcedureList(
					const char *wild);
		bool		getProcedureList(
					const char *wild,
					sqlrclientlistformat_t listformat);
		void		clearBindsDuringPrepare();
		void		dontClearBindsDuringPrepare();
		void		lazyFetch();
		void		dontLazyFetch();
		void		defineInputOutputBindString(
						const char *variable,
						const char *value,
						uint32_t length);
		void		defineInputOutputBindInteger(
						const char *variable,
						int64_t value);
		void		defineInputOutputBindDouble(
						const char *variable,
						double value,
						uint32_t precision,
						uint32_t scale);
		void		defineInputOutputBindDate(
						const char *variable,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative);
		void		defineOutputBindBlob(const char *variable,
						const char *value,
						uint32_t size);
		void		defineOutputBindClob(const char *variable,
						const char *value,
						uint32_t size);
		void		defineInputOutputBindGeneric(
						const char *variable,
						sqlrclientbindvartype_t type,
						const char *strvalue,
						int64_t intvalue,
						double doublevalue,
						uint32_t precision,
						uint32_t scale,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative,
						uint32_t valuesize);
		const char	*getInputOutputBindString(const char *variable);
		uint32_t	getInputOutputBindLength(const char *variable);
		int64_t		getInputOutputBindInteger(const char *variable);
		double		getInputOutputBindDouble(const char *variable);
		bool		getInputOutputBindDate(const char *variable,
							int16_t *year,
							int16_t *month,
							int16_t *day,
							int16_t *hour,
							int16_t *minute,
							int16_t *second,
							int32_t *microsecond,
							const char **tz,
							bool *isnegative);
		const char	*getInputOutputBindBlob(const char *variable);
		const char	*getInputOutputBindClob(const char *variable);
		const char	*getColumnTable(const char *col);
		const char	*getColumnTable(uint32_t col);

	friend class sqlrconnection;
