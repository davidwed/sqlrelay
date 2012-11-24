// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCMDCSTAT_H
#define SQLRCMDCSTAT_H

#include <sqlrquery.h>

class sqlrcmdcstat : public sqlrquery {
	public:
			sqlrcmdcstat(rudiments::xmldomnode *parameters);
		bool	match(const char *querystring, uint32_t querylength);
		sqlrquerycursor	*getCursor(sqlrconnection_svr *conn);
};

class sqlrcmdcstatcursor : public sqlrquerycursor {
	public:
			sqlrcmdcstatcursor(sqlrconnection_svr *sqlrcon,
					rudiments::xmldomnode *parameters);

		bool		executeQuery(const char *query,
						uint32_t length);
		uint32_t	colCount();
		const char * const	*columnNames();
		const char		*getColumnName(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob, bool *null);
	private:
		uint64_t	currentrow;

};

#endif
