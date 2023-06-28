// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

	private:
		void		deleteColumns();
		uint64_t	countColumns(const char * const *columns);
		void		copyColumns(const char * const *columns);

		void	getValidColumnName(const char *c,
						const char **col,
						size_t *collen);
		void	bind(const char *bindformat,
					const char * const *columns,
					const char * const *values);

		bool	doReadDelegate(const char *where,
					const char *orderby,
					uint64_t skip);
		bool	doUpdateDelegate(const char * const *columns,
					const char * const *values,
					const char *where);
		bool	doDeleteDelegate(const char *where);

		bool	buildWhere(const char *criteria,
					stringbuffer *wherestr,
					bool containspartial);
		bool	buildOrderBy(const char *sort,
					stringbuffer *orderbystr,
					bool containspartial);
		bool	buildClause(const char *domstr,
					stringbuffer *strb,
					bool where,
					bool containspartial);
		bool	buildJsonWhere(domnode *criteria,
					stringbuffer *wherestr,
					bool containspartial);
		bool	buildXmlWhere(domnode *criteria,
					stringbuffer *wherestr,
					bool containspartial);
		bool	buildJsonOrderBy(domnode *sort,
					stringbuffer *orderbystr,
					bool containspartial);
		bool	buildXmlOrderBy(domnode *sort,
					stringbuffer *orderbystr,
					bool containspartial);

		sqlrconnection	*con;
		sqlrcursor	*cur;

		char	*tbl;
		char	*idsequence;
		char	*primarykey;
		char	*autoinc;

		char	**columns;

		stringbuffer	error;

		stringbuffer	createquery;
		stringbuffer	readquery;
		stringbuffer	updatequery;
		stringbuffer	deletequery;

		bool	readcontainspartialwhere;
		bool	readcontainspartialorderby;
		bool	updatecontainspartialwhere;
		bool	deletecontainspartialwhere;

		scalar<uint64_t>			ars;
		linkedlist<uint64_t>			arl;
		dictionary<const char *, uint64_t>	ard;
		table<uint64_t>				art;

		sqlrscalar		ffs;
		sqlrrowlist		frl;
		sqlrrowdictionary	frd;
		sqlrresultsetlist	fcl;
		sqlrresultsettable	rst;

		memorypool	m;

		xmldom	x;
		jsondom	j;
