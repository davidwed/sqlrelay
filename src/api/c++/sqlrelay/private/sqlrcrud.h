// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

	private:
		void		deleteColumns();
		uint64_t	countColumns(const char * const *columns);
		void		copyColumns(const char * const *columns);

		void		bind(const char *bindformat,
					const char * const *columns,
					const char * const *values);

		bool	buildWhere(const char *criteria,
					stringbuffer *wherestr);
		bool	buildOrderBy(const char *sort,
					stringbuffer *orderbystr);
		bool	buildClause(const char *domstr,
					stringbuffer *strb,
					bool where);
		bool	buildJsonWhere(domnode *criteria,
					stringbuffer *wherestr);
		bool	buildXmlWhere(domnode *criteria,
					stringbuffer *wherestr);
		bool	buildJsonOrderBy(domnode *sort,
					stringbuffer *orderbystr);
		bool	buildXmlOrderBy(domnode *sort,
					stringbuffer *orderbystr);

		sqlrconnection	*con;
		sqlrcursor	*cur;

		char	*table;
		char	*idsequence;
		char	*primarykey;
		char	*autoinc;

		char	**columns;

		stringbuffer	error;

		stringbuffer	createquery;
		stringbuffer	readquery;
		stringbuffer	updatequery;
		stringbuffer	deletequery;

		sqlrscalar		scl;
		sqlrrowlinkedlist	rlst;
		sqlrrowdictionary	rdct;
		sqlrresultsetlinkedlist	rslst;
		sqlrresultsettable	rstbl;

		xmldom	x;
		jsondom	j;
