
	private:
		bool	column(const char *name, bool quoted);
		bool	headerEnd();
		bool	bodyStart();
		bool	rowStart();
		bool	field(const char *value, bool quoted);
		bool	rowEnd();
		bool	bodyEnd();

		void	massageField(stringbuffer *strb, const char *field);

		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;

		stringbuffer	query;
		char		*table;
		bool		ignorecolumns;
		uint32_t	colcount;
		stringbuffer	columns;
		bool		*numbercolumn;
		uint32_t	currentcol;
		bool		foundfieldtext;
		uint32_t	fieldcount;
		uint64_t	rowcount;
		uint64_t	commitcount;
		uint64_t	committedcount;
		bool		verbose;
		const char	*dbtype;
