// Copyright (c) 1999-2001  David Muse
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
		void	initVariables();
		void	deleteVariables();

		void	initQueryBuffer(uint32_t querylength);
		bool	sendQueryInternal(const char *query);
		bool	getList(uint16_t command,
				const char *table, const char *wild);
		void	sendCursorStatus();
		void	performSubstitutions();
		void	validateBindsInternal();
		void	sendInputBinds();
		void	sendOutputBinds();
		void	sendGetColumnInfo();
		void	defineOutputBindGeneric(const char *variable,
						bindtype type,
						uint32_t valuesize);
		void	stringVar(bindvar *var,
					const char *variable,
					const char *value);
		void	stringVar(bindvar *var,
					const char *variable,
					const char *value,
					uint32_t valuesize);
		void	integerVar(bindvar *var,
					const char *variable,
					int64_t value);
		void	doubleVar(bindvar *var, const char *variable, 
						double value,
						uint32_t precision,
						uint32_t scale);
		void	dateVar(bindvar *var, const char *variable,
						int16_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz);
		void	lobVar(bindvar *var, const char *variable,
					const char *value, uint32_t size,
					bindtype type);
		bindvar	*findVar(const char *variable,
					bindvar *vars,
					uint16_t count);
		void	initVar(bindvar *var, const char *variable);
		void	performSubstitution(stringbuffer *buffer,
							uint16_t which);
		bool	runQuery(const char *query);
		bool	processResultSet(bool getallrows, uint64_t rowtoget);

		int32_t	getString(char *string, int32_t size);
		int32_t	getShort(uint16_t *integer);
		int32_t	getLong(uint32_t *integer);
		int32_t	getLongLong(uint64_t *integer);
		int32_t	getDouble(double *value);

		uint16_t	getErrorStatus();

		bool	getCursorId();
		bool	getSuspended();
		bool	parseColumnInfo();
		bool	parseOutputBinds();
		bool	parseData();
		void	setError(const char *err);
		void	getErrorFromServer();
		void	handleError();

		bool	skipAndFetch(bool getallrows, uint64_t rowtoget);
		bool	skipRows(bool getallrows, uint64_t rowtoget);
		void	fetchRows();

		void	startCaching();
		void	cacheError();
		void	cacheNoError();
		void	cacheColumnInfo();
		void	cacheOutputBinds(uint32_t count);
		void	cacheData();
		void	finishCaching();
 
		bool	fetchRowIntoBuffer(bool getallrows,
					uint64_t row, uint64_t *rowbufferindex);

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
		column	*getColumn(uint32_t index);
		column	*getColumn(const char *name);
		column	*getColumnInternal(uint32_t index);
		char	*getColStorage(int32_t length);
		void	createColumnBuffers();


		bool		resumed;
		bool		cached;

		// query
		char		*querybuffer;
		const char	*queryptr;
		uint32_t	querylen;
		char		*fullpath;
		bool		reexecute;

		// substitution variables
		bindvar		subvars[MAXVAR];
		int16_t		subcount;
		bool		dirtysubs;

		// bind variables
		bindvar		inbindvars[MAXVAR];
		uint16_t	inbindcount;
		bindvar		outbindvars[MAXVAR];
		uint16_t	outbindcount;
		bool		validatebinds;
		bool		dirtybinds;

		// result set
		uint64_t	rsbuffersize;
		uint16_t	sendcolumninfo;
		uint16_t	sentcolumninfo;

		uint16_t	suspendresultsetsent;
		bool		endofresultset;

		uint16_t	columntypeformat;
		uint32_t	colcount;
		uint32_t	previouscolcount;

		columncase	colcase;

		column		*columns;
		column		*extracolumns;
		memorypool	*colstorage;
		char		**columnnamearray;

		uint64_t	firstrowindex;
		uint64_t	rowcount;
		uint64_t	previousrowcount;
		uint16_t	knowsactualrows;
		uint64_t	actualrows;
		uint16_t	knowsaffectedrows;
		uint64_t	affectedrows;

		row		**rows;
		row		**extrarows;
		memorypool	*rowstorage;
		row		*firstextrarow;
		char		***fields;
		uint32_t	**fieldlengths;

		bool		returnnulls;

		// result set caching
		bool		cacheon;
		int32_t		cachettl;
		char		*cachedestname;
		char		*cachedestindname;
		file		*cachedest;
		file		*cachedestind;
		file		*cachesource;
		file		*cachesourceind;

		// error
		int64_t		errorno;
		char		*error;

		// copy references flag
		bool		copyrefs;

		// parent connection
		sqlrconnection	*sqlrc;

		// next/previous pointers
		sqlrcursor	*next;
		sqlrcursor	*prev;

		// cursor id
		uint16_t	cursorid;
		bool		havecursorid;

	public:
		sqlrcursor(sqlrconnection *sqlrcon, bool copyreferences);
		bool		outputBindCursorIdIsValid(const char *variable);
		sqlrcursor	*getOutputBindCursor(const char *variable,
								bool copyrefs);
		uint16_t	getOutputBindCursorId(const char *variable);
		void		attachToBindCursor(uint16_t bindcursorid);

	friend class sqlrconnection;
