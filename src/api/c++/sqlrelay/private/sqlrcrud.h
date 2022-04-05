// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

	private:
		void		deleteColumns();
		uint64_t	countColumns(const char * const *columns);
		void		copyColumns(const char * const *columns);

		void	buildWhere(const char * const *criteria,
						stringbuffer *wherestr);
		void	buildOrderBy(const char * const *sort,
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
