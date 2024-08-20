// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

		bool	getSupportsLimit();

		virtual void	clearFlagsAndCounts();

		void	setEmptyRow(bool emptyrow);
		bool	getEmptyRow();

		void	setColumnNameBuffer(const char *value);
		char	*getColumnNameBuffer();
		void	clearColumnNameBuffer();
		void	freeColumnNameBuffer();

		void	setColumnName(uint64_t index, char *value);
		char	*getColumnName(uint64_t index);
		uint64_t	getColumnNameCount();
		void	clearColumnNames();

		void	setFieldBuffer(const char *value);
		char	*getFieldBuffer();
		void	clearFieldBuffer();
		void	freeFieldBuffer();

		void	setField(uint64_t index, char *value);
		char	*getField(uint64_t index);
		uint64_t	getFieldCount();
		void	clearFields();

		void	setQuoteField(uint64_t index, bool quote);
		bool	getQuoteField(uint64_t index);
		void	clearQuoteFields();

		void	appendToQueryBuffer(const char *str);
		void	appendToQueryBuffer(const char ch);
		const char	*getQueryBufferString();
		void	clearQueryBuffer();

		virtual bool	startProcessingImport();
		virtual bool	startProcessingColumns();
		virtual bool	processColumnName();
		virtual bool	processPrimaryKeyAndStaticColumns();
		virtual bool	determineColumnTypes();
		virtual bool	endProcessingColumns();
		virtual bool	startProcessingRows();
		virtual bool	startProcessingRow();
		virtual bool	initialBegin();
		virtual bool	processField();
		virtual char	*massageValue(const char *value,
							bool isnumeric,
							bool isdatetime);
		virtual char	*unescapeValue(const char *value);
		virtual bool	endProcessingRow();
		virtual bool	insertRow();
		virtual bool	periodicCommit();
		virtual bool	finalCommit();
		virtual bool	endProcessingRows();
		virtual bool	endProcessingImport();

		virtual bool	systemError();

	private:
		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;

		bool		insertprimarykey;
		char		*primarykeycolumnname;
		uint32_t	primarykeycolumnindex;
		char		*primarykeysequence;

		dictionary<uint32_t, char *>	staticvaluecolumnnames;
		dictionary<uint32_t, char *>	staticvalues;

		char		*dbtype;
		bool		supportslimit;

		char		*objectname;

		bool		ignorecolumns;
		bool		ignorecolumnswithemptynames;

		bool		ignoreemptyrows;

		bool		lowercasecolumnnames;
		bool		uppercasecolumnnames;

		bool		reformatdatetime;
		bool		ddmm;
		bool		yyyyddmm;
		const char	*datedelimiters;
		uint16_t	nocenturythreshold;
		uint16_t	lastcenturythreshold;
		const char	*datetimeformat;

		logger		*lg;
		uint8_t		coarseloglevel;
		uint8_t		fineloglevel;
		uint32_t	logindent;
		bool		logerrors;

		char					*colnamebuffer;
		dictionary<const char *, const char *>	columnmap;
		dynamicarray<char *>			columnnames;

		char					*fieldbuffer;
		dictionary<const char *, const char *>	fieldmap;
		dynamicarray<char *>			fields;
		dynamicarray<bool>			quotefields;

		bool				excluderow;
		uint64_t			currentrow;
		uint32_t			currentcol;
		char				*currentcolname;
		dictionary<uint32_t, bool>	numericcolumn;
		dictionary<uint32_t, bool>	datetimecolumn;
		char				*currentfield;
		bool				emptyrow;
		uint64_t			importedrowcount;
		uint64_t			commitcount;

		stringbuffer	query;
