// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information.

	private:
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

		void	initQueryBuffer();
		int	sendQueryInternal(const char *query);
		void	validateBindsInternal(const char *query);
		void	sendInputBinds();
		void	sendOutputBinds();
		void	sendGetColumnInfo();
		void	defineOutputBindGeneric(const char *variable,
						bindtype type,
						unsigned long valuesize);
		void	stringVar(bindvar *var, const char *variable, const char *value);
		void	longVar(bindvar *var, const char *variable, long value);
		void	doubleVar(bindvar *var, const char *variable, 
						double value,
						unsigned short precision,
						unsigned short scale);
		void	lobVar(bindvar *var, const char *variable,
					const char *value, unsigned long size,
					bindtype type);
		void	initVar(bindvar *var, const char *variable);
		void	performSubstitution(stringbuffer *buffer, int which);
		int	runQuery(const char *query);
		void	abortResultSet();
		int	processResultSet(int rowtoget);

		int	getString(char *string, int size);
		int	getShort(unsigned short *integer);
		int	getLong(unsigned long *integer);

		int	noError();
		int	getCursorId();
		int	getSuspended();
		int	parseColumnInfo();
		int	parseOutputBinds();
		int	parseData();
		void	setError(const char *err);
		void	getErrorFromServer();
		void	handleError();

		int	skipAndFetch(int rowtoget);
		void	fetchRows();
		int	skipRows(int rowtoget);

		void	startCaching();
		void	cacheNoError();
		void	cacheColumnInfo();
		void	cacheOutputBinds(int count);
		void	cacheData();
		void	finishCaching();
		void	suspendCaching();
 
		int	fetchRowIntoBuffer(int row);

		void	createColumnArrays();
		void	createExtraRowArray();
		void	createFields();
		void	createFieldLengths();

		char		*getFieldInternal(int row, int col);
		unsigned long	getFieldLengthInternal(int row, int col);

		char	*getRowStorage(int length);
		void	createRowBuffers();
		column	*getColumn(int index);
		char	*getColStorage(int length);
		void	createColumnBuffers();


		int		resumed;
		int		cached;

		// query
		char		*querybuffer;
		char		*queryptr;
		int		querylen;
		char		*fullpath;
		int		reexecute;

		// substitution variables
		bindvar		subvars[MAXVAR];
		int		subcount;

		// bind variables
		bindvar		inbindvars[MAXVAR];
		int		inbindcount;
		bindvar		outbindvars[MAXVAR];
		int		outbindcount;
		int		validatebinds;

		// result set
		long		rsbuffersize;
		unsigned short	sendcolumninfo;
		unsigned short	sentcolumninfo;

		unsigned short	suspendresultsetsent;
		unsigned short	endofresultset;

		unsigned short	columntypeformat;
		unsigned long	colcount;
		unsigned long	previouscolcount;

		columncase	colcase;

		column		*columns;
		column		*extracolumns;
		memorypool	*colstorage;
		char		**columnnamearray;

		unsigned long	firstrowindex;
		long		rowcount;
		long		previousrowcount;
		unsigned short	knowsactualrows;
		unsigned long	actualrows;
		unsigned short	knowsaffectedrows;
		unsigned long	affectedrows;

		row		**rows;
		row		**extrarows;
		memorypool	*rowstorage;
		row		*firstextrarow;
		int		getrowcount;
		int		getrowlengthcount;
		char		***fields;
		unsigned long	**fieldlengths;

		int		returnnulls;

		// result set caching
		int		cacheon;
		int		cachettl;
		char		*cachedestname;
		char		*cachedestindname;
		file		*cachedest;
		file		*cachedestind;
		file		*cachesource;
		file		*cachesourceind;

		// error
		char		*error;

		// copy references flag
		int		copyrefs;

		// parent connection
		sqlrconnection	*sqlrc;

		// next/previous pointers
		sqlrcursor	*next;
		sqlrcursor	*prev;

		// cursor id
		unsigned short	cursorid;

	public:
		void		copyReferences();
		short		getOutputBindCursorId(const char *variable);
		void		attachToBindCursor(short bindcursorid);

	friend class sqlrconnection;
