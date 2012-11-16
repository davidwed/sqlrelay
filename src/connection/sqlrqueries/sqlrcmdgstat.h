// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCMDGSTAT_H
#define SQLRCMDGSTAT_H

#include <sqlrquery.h>

class sqlrcmdgstat : public sqlrquery {
	public:
			sqlrcmdgstat(rudiments::xmldomnode *parameters);

		bool	init(sqlrconnection_svr *sqlrcon);
		bool	match(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *querystring,
						uint32_t querylength);

		bool	executeQuery(const char *query,
						uint32_t length);
		bool	errorMessage(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		uint64_t	affectedRows();
		uint32_t	colCount();
		const char * const *	columnNames();
		void		returnColumnInfo();
		bool		noRowsToReturn();
		bool		skipRow();
		bool		fetchRow();
		bool		returnRow();
		bool		nextRow();
		void		getField(uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null);
		bool		getColumnNameList(
					rudiments::stringbuffer *output);
};

#endif
