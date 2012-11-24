// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCMDGSTAT_H
#define SQLRCMDGSTAT_H

#include <sqlrquery.h>

class sqlrcmdgstat : public sqlrquery {
	public:
			sqlrcmdgstat(rudiments::xmldomnode *parameters);
		bool	match(const char *querystring, uint32_t querylength);
		sqlrquerycursor	*getCursor(sqlrconnection_svr *conn);
};

class sqlrcmdgstatcursor : public sqlrquerycursor {
	public:
			sqlrcmdgstatcursor(sqlrconnection_svr *sqlrcon,
					rudiments::xmldomnode *parameters);

};

#endif
