// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRCMD_H
#define SQLRCMD_H

#include <sqlrquery.h>

class sqlrcmd : public sqlrquery {
	public:
			sqlrcmd(rudiments::xmldomnode *parameters);

		virtual bool	init(sqlrconnection_svr *sqlrcon);
		virtual bool	match(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *querystring);
};

#endif
