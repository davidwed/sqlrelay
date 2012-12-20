// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRQUERY_H
#define SQLRQUERY_H

#include <sqlrcursor.h>
#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

// for return values of columnTypeFormat
#include <datatypes.h>

class sqlrquerycursor;

class sqlrquery {
	public:
			sqlrquery(rudiments::xmldomnode *parameters);
		virtual	~sqlrquery();

		virtual bool	match(const char *querystring,
						uint32_t querylength);
		virtual sqlrquerycursor	*getCursor(sqlrconnection_svr *sqlrcon);
	protected:
		rudiments::xmldomnode	*parameters;
};

class sqlrquerycursor : public sqlrcursor_svr {
	public:
			sqlrquerycursor(sqlrconnection_svr *conn,
					rudiments::xmldomnode *parameters);
		virtual	~sqlrquerycursor();
		virtual sqlrquerytype_t	queryType(const char *query,
							uint32_t length);
	protected:
		rudiments::xmldomnode	*parameters;
};

#endif
