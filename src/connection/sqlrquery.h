// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRQUERY_H
#define SQLRQUERY_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqlrquery {
	public:
			sqlrquery(rudiments::xmldomnode *parameters);
		virtual	~sqlrquery();

		virtual bool	init(sqlrconnection_svr *sqlrcon);
		virtual bool	match(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *querystring);
	protected:
		rudiments::xmldomnode	*parameters;
};

#endif
