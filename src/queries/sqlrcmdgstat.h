// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCMDGSTAT_H
#define SQLRCMDGSTAT_H

#include <sqlrcontroller.h>
#include <sqlrconnection.h>
#include <sqlrquery.h>

class sqlrcmdgstat : public sqlrquery {
	public:
			sqlrcmdgstat(rudiments::xmldomnode *parameters);
		bool	match(const char *querystring, uint32_t querylength);
		sqlrquerycursor	*getCursor(sqlrconnection_svr *conn);
};

#define GSTAT_KEY_LEN	40
#define GSTAT_VALUE_LEN	40

struct gs_result_row {
	char	key[GSTAT_KEY_LEN+1];
	char	value[GSTAT_VALUE_LEN+1];
};

class sqlrcmdgstatcursor : public sqlrquerycursor {
	public:
			sqlrcmdgstatcursor(sqlrconnection_svr *sqlrcon,
					rudiments::xmldomnode *parameters);

		bool		executeQuery(const char *query,
						uint32_t length);
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob, bool *null);
	private:
		void	setGSResult(const char *key,
					int32_t value, uint16_t i);
		void	setGSResult(const char *key,
					const char *value, uint16_t i);

		uint64_t	rowcount;
		uint64_t	currentrow;

		gs_result_row	gs_resultset[60];

};

#endif
