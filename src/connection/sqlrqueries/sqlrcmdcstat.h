// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCMDCSTAT_H
#define SQLRCMDCSTAT_H

#include <sqlrquery.h>

class sqlrcmdcstat : public sqlrquery {
	public:
			sqlrcmdcstat(rudiments::xmldomnode *parameters);

		bool	init(sqlrconnection_svr *sqlrcon);
		bool	match(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *querystring,
						uint32_t querylength);
};

#endif
