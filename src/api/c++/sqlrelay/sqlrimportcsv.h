// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTCSV_H
#define SQLRIMPORTCSV_H

#include <sqlrelay/private/sqlrimportcsvincludes.h>

class sqlrimportcsv : public csvsax {
	public:
			sqlrimportcsv(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur,
					uint64_t commitcount,
					bool verbose,
					const char *dbtype);
			~sqlrimportcsv();

		void	setTable(const char *table);
		void	setIgnoreColumns(bool ignorecolumns);
		bool	parseFile(const char *filename);

	#include <sqlrelay/private/sqlrimportcsv.h>
};

#endif
