// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef BENCH_H
#define BENCH_H

#include <rudiments/parameterstring.h>

class benchconnection {
	public:
			benchconnection(const char *connectstring,
					const char *dbtype);
		virtual	~benchconnection();

		virtual void	setRowCount(uint64_t rowcount);
		virtual void	setColumnCount(uint32_t columncount);

		virtual	void	buildQueries();

		virtual	bool	connect()=0;
		virtual	bool	disconnect()=0;

		const char	*getParam(const char *param);

		const char	*getDbType();

		const char	*getCreateQuery();
		const char	*getDropQuery();
		const char	*getInsertQuery();
		const char	*getUpdateQuery();
		const char	*getDeleteQuery();
		const char	*getSelectQuery();

	private:
		void	buildOracleQueries();

		parameterstring	pstring;
		const char	*dbtype;

		uint64_t	rowcount;
		uint32_t	columncount;

		char	*createquery;
		char	*dropquery;
		char	*insertquery;
		char	*updatequery;
		char	*deletequery;
		char	*selectquery;
};

class benchcursor {
	public:
			benchcursor(benchconnection *bcon);
		virtual	~benchcursor();

		virtual	bool	createTable()=0;
		virtual	bool	dropTable()=0;

		virtual	bool	insertQuery()=0;
		virtual	bool	updateQuery()=0;
		virtual	bool	deleteQuery()=0;
		virtual	bool	selectQuery()=0;

	protected:
		benchconnection	*bcon;
};

#endif
