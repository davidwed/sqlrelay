// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

	private:
		void		deleteColumns();
		uint64_t	countColumns(const char * const *columns);
		void		copyColumns(const char * const *columns);

		bool	buildWhere(const char *criteria,
					stringbuffer *wherestr);
		bool	buildOrderBy(const char *sort,
					stringbuffer *orderbystr);
		bool	buildClause(const char *domstr,
					stringbuffer *strb,
					bool where);
		bool	buildJSONWhere(domnode *criteria,
					stringbuffer *wherestr);
		bool	buildXMLWhere(domnode *criteria,
					stringbuffer *wherestr);
		bool	buildJSONOrderBy(domnode *sort,
					stringbuffer *orderbystr);
		bool	buildXMLOrderBy(domnode *sort,
					stringbuffer *orderbystr);

		sqlrconnection	*con;
		sqlrcursor	*cur;

		char	*table;
		char	*primarykey;
		char	*autoinc;

		char	**columns;

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
